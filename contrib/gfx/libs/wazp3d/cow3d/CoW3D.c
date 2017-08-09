/* Author: Alain Thellier - Paris - 2010 . See ReadMe for more infos					*/
/* Cow3D-3.c: Cow V2 10 nov 2010 : now can draw cow as lines/points + zbuffer to grey		*/
/* v3.0 Sept 2011 : Draw FPS correctly on Aros too. Now works with Wazp3D using "hard overlay"	*/


#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#ifdef STATWAZP3D
#include "Warp3D_protos.h"
#include "cybergraphics.h"
#else
#include <proto/Warp3D.h>
#include <cybergraphx/cybergraphics.h>
#endif

#include <proto/cybergraphics.h>

/*==================================================================*/
#include "Cow3D.h"				/* 3d object*/
UBYTE  progname[]={"CoW3D V3"};
#define LARGE 640					/* window size */
#define HIGH  480
#define NLOOP(nbre) for(n=0;n<nbre;n++)
#define MLOOP(nbre) for(m=0;m<nbre;m++)
#define SPEED 5					/* object speed */
#define FRAMESCOUNT (360/SPEED)		/* frames count to rotate fully */
#define MAXPRIM (3*1000)			/* Wazp3D cant draw bigger primitives  so split the object in several drawing*/
#include "patches_for_Warp3D.c"
/*==================================================================*/
struct point3D{
	float x,y,z,u,v,w;
	UBYTE RGBA[4];
};
/*==================================================================*/
struct object3D{
	struct point3D  *P;		/* original points */
	struct point3D  *P2;		/* points rotated/transformed to screen */
	ULONG Pnb;				/* points counts(french "points nombre" */
	ULONG *PI;				/* points' indices that define the triangles */
	ULONG PInb;				/* PI counts(french "PI nombre" */
	UBYTE *picture;			/* texture data */
	W3D_Texture *tex;			/* 3d-texture */
};
/*==================================================================*/
struct Screen  *screen		=NULL;
struct Window  *window		=NULL;
struct BitMap  *bm		=NULL;
struct Library *Warp3DBase	=NULL;
struct Library *CyberGfxBase	=NULL;
W3D_Context *context		=NULL;
W3D_Scissor scissor = {0,0,LARGE,HIGH};
struct RastPort bufferrastport;
ULONG result,ModeID,flags,ScreenBits;
BOOL  closed=FALSE;
BOOL greyreadz=FALSE;
BOOL colored=TRUE;
BOOL showfps=TRUE;
BOOL zbuffer=TRUE;
BOOL zupdate=TRUE;
BOOL hideface=TRUE;
BOOL rotate=TRUE;
BOOL drawcow=TRUE;
UBYTE drawmode='t';
BOOL bigpoint=FALSE;
BOOL lintest=FALSE;
BOOL ztest=FALSE;
BOOL buffered=FALSE;
ULONG zmode=W3D_Z_LESS;
BOOL optimroty=FALSE;
float ViewMatrix[16];
struct object3D Obj;
float RotY=0.0;
ULONG FramesCounted=0;
ULONG FPS=0;
UBYTE FpsText[200];
UBYTE zname[5];
struct object3D *CowObj;
struct object3D *CosmosObj;
struct object3D *QuadObj;
struct point3D  *BufferedP=NULL;		/* store all transformed points */
UBYTE Pdone[FRAMESCOUNT];
ULONG srcfunc=0;
ULONG dstfunc=0;
ULONG srcdstfunc=0;
ULONG texenvmode=1;
/*==================================================================*/
void DrawEleWarp3D(struct point3D  *P,ULONG *PI,ULONG PInb,ULONG primitive,W3D_Texture *tex)
{
void *VertexPointer;
void *TexCoordPointer;
void *ColorPointer;
UWORD stride=sizeof(struct point3D);
UWORD off_v,off_w;

	W3D_WaitIdle(context);		/* dont modify points or pointers during drawing */
/* set arrays pointers */
	VertexPointer=	(void *)&(P->x);
	TexCoordPointer=(void *)&(P->u);
	ColorPointer=	(void *)&(P->RGBA);
	off_v=(UWORD)( (IPTR)&(P->v) - (IPTR)&(P->u));
	off_w=(UWORD)( (IPTR)&(P->w) - (IPTR)&(P->u));
	result=W3D_VertexPointer(context,VertexPointer,stride,W3D_VERTEX_F_F_F, 0);
	result=W3D_TexCoordPointer(context,TexCoordPointer,stride,0, off_v, off_w,W3D_TEXCOORD_NORMALIZED);
	result=W3D_ColorPointer(context,ColorPointer,stride,W3D_COLOR_UBYTE ,W3D_CMODE_RGBA,0);
/* lock hardware then draw */
	if( W3D_SUCCESS != W3D_LockHardware(context) ) return;

	W3D_BindTexture(context,0,tex);
	W3D_DrawElements(context,primitive,W3D_INDEX_ULONG,PInb,PI);	/* draw with warp3d */
	W3D_Flush(context);
	W3D_WaitIdle(context);

	W3D_UnLockHardware(context);
}
/*==================================================================*/
void PtoV(struct point3D  *P,W3D_Vertex *v)
{
	v->x=P->x; v->y=P->y; v->z=P->z; v->w=P->w;
	v->u=P->u*TEXSIZE; v->v=P->v*TEXSIZE;
	v->color.r=(float)P->RGBA[0]/255.0;
	v->color.g=(float)P->RGBA[1]/255.0;
	v->color.b=(float)P->RGBA[2]/255.0;
	v->color.a=(float)P->RGBA[3]/255.0;
}
/*==================================================================*/
void DrawTriWarp3D(struct point3D  *P,ULONG *PI,ULONG PInb,W3D_Texture *tex)
{
W3D_Triangle tri;
register ULONG n;

	if( W3D_SUCCESS != W3D_LockHardware(context) ) return;

	tri.tex=tex;
	NLOOP(PInb/3)
	{
	PtoV( &P[PI[n*3+0]], &tri.v1 );
	PtoV( &P[PI[n*3+1]], &tri.v2 );
	PtoV( &P[PI[n*3+2]], &tri.v3 );
 	W3D_DrawTriangle(context, &tri);
	}

	W3D_UnLockHardware(context);
}
/*==================================================================*/
void DrawPoiWarp3D(struct point3D  *P,ULONG *PI,ULONG PInb,W3D_Texture *tex)
{
W3D_Point point;
register ULONG n;

	if( W3D_SUCCESS != W3D_LockHardware(context) ) return;

	point.tex=tex;
	point.pointsize=1.0;
	if(bigpoint) point.pointsize=8.0;
	NLOOP(PInb)
	{
	PtoV( &P[PI[n]], &point.v1 );
 	PatchW3D_DrawPoint(context,&point);
	}

	W3D_UnLockHardware(context);
}
/*==================================================================*/
void DrawLinWarp3D(struct point3D  *P,ULONG *PI,ULONG PInb,W3D_Texture *tex)
{
W3D_Vertex v[3];
W3D_Lines lines;
register ULONG n;

	if( W3D_SUCCESS != W3D_LockHardware(context) ) return;

	lines.v=v;
	lines.vertexcount=3;
	lines.tex=tex;
	lines.linewidth=1.0;
	if(bigpoint) lines.linewidth=4.0;
	NLOOP(PInb/3)
	{
	PtoV( &P[PI[n*3+0]], &v[0] );
	PtoV( &P[PI[n*3+1]], &v[1] );
	PtoV( &P[PI[n*3+2]], &v[2] );
 	PatchW3D_DrawLineLoop(context,&lines);
	}

	W3D_UnLockHardware(context);
}

/*==================================================================*/
void DrawObject(struct object3D *O)
{
ULONG n,nb,rest;

	if(drawmode=='t')
		{DrawTriWarp3D(O->P2,O->PI,O->PInb,O->tex);return;}
	if(drawmode=='p')
		{DrawPoiWarp3D(O->P2,O->PI,O->PInb,O->tex);return;}
	if(drawmode=='l')
		{DrawLinWarp3D(O->P2,O->PI,O->PInb,O->tex);return;}
	if(drawmode=='e')
		{
		nb=O->PInb/MAXPRIM;
		rest=O->PInb - nb*MAXPRIM;
		NLOOP(nb)
			DrawEleWarp3D(O->P2,&O->PI[MAXPRIM*n],MAXPRIM,W3D_PRIMITIVE_TRIANGLES,O->tex);
		DrawEleWarp3D(O->P2,&O->PI[MAXPRIM*nb],rest,W3D_PRIMITIVE_TRIANGLES,O->tex);
		}
}
/*=================================================================*/
void SetP(struct point3D  *P,float x, float y, float z,float w,float u, float v,UBYTE r,UBYTE g,UBYTE b,UBYTE a)
{
	if(P==NULL) return;
	P->x=x; P->y=y; P->z=z;	P->w=w; P->u=u; P->v=v;
	P->RGBA[0]=r;P->RGBA[1]=g;P->RGBA[2]=b;P->RGBA[3]=a;
}
/*==================================================================*/
void SetQuad(struct object3D *Quad,float x,float y,float z,float large,float high,UBYTE r,UBYTE g,UBYTE b,UBYTE a)
{
	Quad->PI[0]=0; /* do the quad as 2 triangles */
	Quad->PI[1]=1;
	Quad->PI[2]=2;
	Quad->PI[3]=0;
	Quad->PI[4]=2;
	Quad->PI[5]=3;

	SetP(&Quad->P2[0],x,y+high,z,1.0/z,0.0,1.0,r,g,b,a);
	SetP(&Quad->P2[1],x+large,y+high,z,1.0/z,1.0,1.0,r,g,b,a);
	SetP(&Quad->P2[2],x+large,y,z,1.0/z,1.0,0.0,r,g,b,a);
	SetP(&Quad->P2[3],x,y,z,1.0/z,0.0,0.0,r,g,b,a);

}
/*==================================================================*/
void SetV(W3D_Vertex *V,float x, float y, float z,float w,float u, float v,UBYTE r,UBYTE g,UBYTE b,UBYTE a)
{
	V->x=x;
	V->y=y;
	V->z=z;
	V->w=w;
	V->u=u*TEXSIZE;
	V->v=v*TEXSIZE;
	V->color.r=(float)r/255.0;
	V->color.g=(float)g/255.0;
	V->color.b=(float)b/255.0;
	V->color.a=(float)a/255.0;
}
/*==================================================================================*/
void MyDumpMem(UBYTE *pt,LONG nb)
{
LONG n;

NLOOP(nb/4)
	{
	printf("[%ld\t][%ld\t] %d\t%d\t%d\t%d\n",(long)pt,(long)4*n,pt[0],pt[1],pt[2],pt[3]);
	pt=&(pt[4]);
	}
}
/*==================================================================*/
void DrawPointLineTestsWarp3D(W3D_Texture *tex)
{
UBYTE r,g,b,a;
float x,y,z,w,u,v,u2,v2;
W3D_Line line;
W3D_Point point;

	if( W3D_SUCCESS != W3D_LockHardware(context) ) return;

	r=255; g=0; b=0; a=255; /* use a simple red */
	u=v=0.0;
	u2=v2=0.9;
	point.tex=tex;


	x=400; y=20; z=0.1; w=1.0/z;
	point.pointsize=1;
	SetV(&point.v1,x,y,z,w,u,v,r,g,b,a);
	result=PatchW3D_DrawPoint(context,&point);
	if(result!=W3D_SUCCESS)
		printf("Cant W3D_DrawPoint size=1\n");

	x=400; y=40; z=0.1; w=1.0/z;
	point.pointsize=5;
	SetV(&point.v1,x,y,z,w,u,v,r,g,b,a);
	result=PatchW3D_DrawPoint(context,&point);
	if(result!=W3D_SUCCESS)
		printf("Cant W3D_DrawPoint size=5\n");

	r=0; g=255; b=0; a=255; /* use a simple green */
	line.tex=tex;
	line.st_enable=FALSE;
	line.st_pattern=0;
	line.st_factor=0;

	x=420; y=20; z=0.1; w=1.0/z;
	line.linewidth=1;
	SetV(&line.v1,x,y,z,w,u,v,r,g,b,a);
	SetV(&line.v2,x+100,y+20,z,w,u2,v2,r,g,b,a);
	result=PatchW3D_DrawLine(context,&line);
	if(result!=W3D_SUCCESS)
		printf("Cant W3D_DrawLine width=1\n");

	x=420; y=40; z=0.1; w=1.0/z;
	line.linewidth=5;
	SetV(&line.v1,x,y,z,w,u,v,r,g,b,a);
	SetV(&line.v2,x+100,y+20,z,w,u2,v2,r,g,b,a);
	result=PatchW3D_DrawLine(context,&line);
	if(result!=W3D_SUCCESS)
		printf("Cant W3D_DrawLine width=5\n");

	W3D_UnLockHardware(context);
}
/*==================================================================*/
void DrawZtests(void)
{
float x,y,z;
float large,high;
ULONG n,m;
UBYTE rgba[4*7]={
0,128,128,255,
0,0,128,255,
128,0,128,255,
128,0,0,255,
128,128,0,255,
0,128,128,255,
255,128,64,255
};
float zvalues[7]={0.10,0.20,0.30,0.40,0.90,0.94,0.98};
double zspan[LARGE];
UBYTE   mask[LARGE];

/* 1: test writezspan */
	if(!w3dpatch)
	if( W3D_SUCCESS != W3D_LockHardware(context) ) return;

	W3D_SetState(context, W3D_TEXMAPPING,W3D_DISABLE);	/* color is enough to test the zbuffer */
	W3D_SetState(context, W3D_BLENDING,	W3D_ENABLE);	/* transparent to see the multipass effect*/

	NLOOP(LARGE)
		{mask[n]=n&1; zspan[n]=0.90;}		/* mask 1 pixel,show 1 pixel...*/

	MLOOP(7)						/* for the 7 test-rectangles */
	NLOOP(50)						/* for the test-rectangle size */
		zspan[n*m]=zvalues[m];			/* fill zspan with same z values as the test-rectangles */

	PatchW3D_WriteZSpan(context,0,40,LARGE,zspan,mask); 	/* draw a zspan in the midle of the test-rectangles */

	NLOOP(LARGE)
		{mask[n]=1; zspan[n]=0.50;}		/* never mask */
	NLOOP(100)
		zspan[n]=n*0.01;

	PatchW3D_WriteZSpan(context,0,42,LARGE,zspan,mask); 	/* draw a zspan in the midle of the test-rectangles */

	if(!w3dpatch)
	W3D_UnLockHardware(context);

/* 2: test zbuffer */
	if( W3D_SUCCESS != W3D_LockHardware(context) ) return;

	large=50.0;
	high =40.0;
	NLOOP(7) /* draw 7 quads as various depth with various colors (z increase with n) */
	{
	x=n*large;
	y=n*2+20;
	z=zvalues[n];
	SetQuad(QuadObj,x,y,z,large+large/3.0,high-n*4,rgba[4*n],rgba[4*n+1],rgba[4*n+2],rgba[4*n+3]);
	DrawObject(QuadObj);
	}

	NLOOP(7) /* do a multi-pass : redraw  a grey transparent square with same depth (sticked to the face) */
	{
	x=n*large;
	y=20+high/2;
	z=zvalues[n];
	SetQuad(QuadObj,x+large/3.0,y,z,large/2,high-n*2,200,200,200,128);
	DrawObject(QuadObj);
	}

	W3D_UnLockHardware(context);
}
/*=============================================================*/
void WriteLineRGBA(APTR RGBA,UWORD y)
{
	WritePixelArray(RGBA,0,0,LARGE*32/8,&bufferrastport,0,y,LARGE,1,RECTFMT_RGBA);
}
/*==================================================================*/
void DrawZvaluesWarp3D(void)
{
ULONG m,n;
UBYTE  RGBA[LARGE][4];
double zspan[LARGE];
UBYTE grey;
typedef const void * (*HOOKEDFUNCTION)(APTR RGBA,UWORD y);	/* doing this forbid the compiler to inline the function */
HOOKEDFUNCTION HookWriteLineRGBA;					/* so calling WritePixelArray dont cause the "missing registers" bug */

	if(!w3dpatch)
	if( W3D_SUCCESS != W3D_LockHardware(context) ) return;

	HookWriteLineRGBA=(HOOKEDFUNCTION)WriteLineRGBA;
	MLOOP(HIGH)
	{
	result=PatchW3D_ReadZSpan(context,0,m,LARGE,zspan);

	NLOOP(LARGE)
		{
		grey=zspan[n]*100.0;
		RGBA[n][0]=RGBA[n][1]=RGBA[n][2]=grey;
		if(result!=W3D_SUCCESS)		/* show not readed lines as red */
			{RGBA[n][0]=255; RGBA[n][1]=0; RGBA[n][2]=0;}
		RGBA[n][3]=255;
		}
	HookWriteLineRGBA(RGBA,m);    /* write a pixels line */
	}

	if(!w3dpatch)
	W3D_UnLockHardware(context);
}
/*=================================================================*/
struct object3D *AddObject(ULONG Pnb,ULONG PInb)
{
struct object3D *O;
ULONG Osize,Psize,PIsize,size;
UBYTE *pt;

	PIsize=PInb*sizeof(ULONG);
	Psize =Pnb *sizeof(struct point3D);
	Osize =	sizeof(struct object3D);
	size=Osize+PIsize+Psize*2; /* define all the object datas in a single malloc */
	pt=malloc(size);

	O=(struct object3D *)pt;	pt=pt+Osize;
	O->PI =(ULONG *)pt;		pt=pt+PIsize;
	O->P 	=(struct point3D *)pt;	pt=pt+Psize;
	O->P2	=(struct point3D *)pt;	pt=pt+Psize;
	O->Pnb =Pnb;
	O->PInb=PInb;
	O->tex=NULL;

	return(O);
}
/*=================================================================*/
void ReadIncludedObject(struct object3D *O,float *V,ULONG *i)
{
struct point3D  *P;
ULONG n;
float x,y,z,w,u,v;
UBYTE r,g,b,a;


	NLOOP(O->PInb)
		O->PI[n]=i[n];
	P=O->P;
	V=points;
	r=g=b=a=255;
	NLOOP(O->Pnb)
		{
		u=V[0]; v=V[1]; x=V[2]; y=V[3]; y=-y; z=V[4]; w=1.0/z;	/* beware z is negated so faces are reverted again in good order*/
		r=x*90+90; g=y*90+90; b=z*90+90;	/* just set various colors a stupid way*/
		SetP(P,x,y,z,w,u,v,r,g,b,a); P++; V=V+5;
		}
}
/*=================================================================*/
void SetMrot(float *M,float R ,UBYTE a,UBYTE b,UBYTE c,UBYTE d)
{	/* define a rotation matrix */
float Pi =3.1416;
WORD n;
	if(M==NULL) return;
	M[0]=M[5]=M[10]=M[15]= 1.0;  	M[1]=M[2]=M[3]=M[4]=M[6]=M[7]=M[8]=M[9]=M[11]=M[12]=M[13]=M[14]=0.0;
	if(R==0.0) return;
	if(R>=360.0)
		{n=R/360.0;  R=R-n*360.0;}
	if(R<0.0)
		{n=1+R/360.0;R=R-n*360.0;}
	R= R / 180.0 * Pi;
	M[a]  =  (float)cos(R);
	M[b]  =  M[a];
	M[c]  =  (float)sin(R);
	M[d]  = -M[c];
}
/*=================================================================*/
void SetMrx(float *M,float x)		/* define X rotation matrix */
	{ SetMrot(M,x,5,10,6,9); }
/*=================================================================*/
void SetMry(float *M,float y)		/* define Y rotation matrix */
	{ SetMrot(M,y,10,0,8,2); }
/*=================================================================*/
void SetMrz(float *M,float z)		/* define Z rotation matrix */
	{ SetMrot(M,z,0,5,1,4); }
/*=================================================================*/
void TransformP(register float *M,struct point3D  *P,LONG Pnb)
{						/* transform points with a given matrix */
register float x;
register float y;
register float z;
register LONG n;

NLOOP(Pnb)
	{
	x=P->x;y=P->y;z=P->z;
	P->x= M[0]*x + M[4]*y+ M[8] *z+ M[12];
	P->y= M[1]*x + M[5]*y+ M[9] *z+ M[13];
	P->z= M[2]*x + M[6]*y+ M[10]*z+ M[14];
	P++;
	}
}
/*=================================================================*/
void YrotateP(register float *M,struct point3D  *P,LONG Pnb)
{						/* y rotate points with a given matrix : optimized TransformP() */
register float x;
register float z;
register float m0 =M[0];
register float m8 =M[8];
register float m2 =M[2];
register float m10=M[10];
register LONG n;

NLOOP(Pnb)
	{
	x=P->x;z=P->z;
	P->x= m0*x +  m8 *z;
	P->z= m2*x +  m10*z;
	P++;
	}
}
/*=================================================================*/
void ProjectP(struct point3D  *P,LONG Pnb)
{						/* project points to screen-coordinates */
register float x;
register float y;
register float z;
register float sizex=LARGE/2.0;
register float sizey=HIGH /2.0;
register float sizez=0.8  /2.0;
register LONG n;

NLOOP(Pnb)
	{
	x=P->x;y=P->y;z=P->z;
	P->x= x*sizex+sizex;
	P->y= y*sizey+sizey;
	P->z= z*sizez+sizez;
	P->w= 1.0/P->z;
	P++;
	}
}
/*=================================================================*/
BOOL LoadFile(UBYTE *buffer, UBYTE *name,ULONG bufferSize)
{						/* load a file in memory */
FILE *fp;
ULONG size=0;

	fp = fopen(name,"rb");
	if(fp == NULL)
		{printf("Cant open file ! <%s>\n",name); return(FALSE); }
	size = fread(buffer,bufferSize,1,fp);
	if(size ==0 )
		{printf("Cant read file ! <%s>\n",name); return(FALSE); }
	fclose(fp);
	return(TRUE);
}
/*=================================================================*/
void LoadTextureWarp3D(struct object3D *O,UBYTE* name,UWORD size,UWORD bits)
{						/* load a RAW rgb bitmap in memory. Make it a Warp3D texture */
ULONG textureSize = size*size*bits/8;
ULONG mode;

	O->tex=NULL;
	O->picture = (UBYTE *) malloc(textureSize);
	if(O->picture==NULL)
		printf("Cant alloc picture! \n");
	if(LoadFile(O->picture,name,textureSize)==FALSE)
		return;
	if (bits==32) mode=W3D_R8G8B8A8; else mode=W3D_R8G8B8;
	O->tex=W3D_AllocTexObjTags(context,&result,W3D_ATO_IMAGE,(IPTR)O->picture,W3D_ATO_FORMAT,mode,W3D_ATO_WIDTH,size,W3D_ATO_HEIGHT,size,TAG_DONE);

	if(result!=W3D_SUCCESS)
		{printf("Cant create tex! \n");return;}

	W3D_SetTexEnv(context,O->tex,W3D_REPLACE,NULL);
}
/*==========================================================================*/
void	 MyDrawText(WORD x,WORD y,UBYTE *text)
{						/* draw a text in the window */
struct RastPort *rp;

	rp=window->RPort;
	SetAPen(rp, 0) ;
	RectFill(rp,x-3,y-9,x+8*strlen(text)+3,y+2);

	SetAPen(rp, 2);
	Move(rp,x-2,y-2+context->yoffset);
	Text(rp,(void*)text, strlen(text));

	SetAPen(rp, 2);
	Move(rp,x,y+context->yoffset);
	Text(rp,(void*)text, strlen(text));

	SetAPen(rp, 1);
	Move(rp,x-1,y-1+context->yoffset);
	Text(rp,(void*)text, strlen(text));

	SetAPen(rp, 1);
}
/*=================================================================*/
static void DoFPS(void)
{					/* count the average "frame per second" */
static clock_t last=0;
clock_t now;
UBYTE drawname[10];

	if(drawmode=='e') strcpy(drawname,"Elem.");
	if(drawmode=='t') strcpy(drawname,"Tris.");
	if(drawmode=='p') strcpy(drawname,"Poin.");
	if(drawmode=='l') strcpy(drawname,"Lin.");

	if(zmode==W3D_Z_NEVER) strcpy(zname,"ZNEVER");
	if(zmode==W3D_Z_LESS) strcpy(zname,"ZLESS");
	if(zmode==W3D_Z_GEQUAL) strcpy(zname,"ZGEQUAL");
	if(zmode==W3D_Z_LEQUAL) strcpy(zname,"ZLEQUAL");
	if(zmode==W3D_Z_GREATER) strcpy(zname,"ZGREATER");
	if(zmode==W3D_Z_NOTEQUAL) strcpy(zname,"ZNOTEQUAL");
	if(zmode==W3D_Z_EQUAL) strcpy(zname,"ZEQUAL");
	if(zmode==W3D_Z_ALWAYS) strcpy(zname,"ZALWAYS");
	sprintf(FpsText,"%s: %ld Fps(Object: %ldtris %ldpoints)(zbuffer%d/zupdate%d(%s) optirot%d buffered%d) as %s(big%d)",progname,(long)FPS,(long)CowObj->PInb/3,(long)CowObj->Pnb,zbuffer,zupdate,zname,optimroty,buffered,drawname,bigpoint);

	if (++FramesCounted >= (FRAMESCOUNT*2)) 			/* ie after the object turned two times  */
	{
      now  = clock();
      FPS = (now - last > 0) ? ((FRAMESCOUNT*2)*CLOCKS_PER_SEC) / (now-last) : 0;
      last = now; FramesCounted = 0;
	printf("%s\n",FpsText);

	if(ztest)
	{
	printf("context->zbuffer:\n");
	MyDumpMem(context->zbuffer,16);
	}
	}

	MyDrawText(3,9,FpsText);
}
/*=================================================================*/
void SwitchDisplayWarp3D(void)
{
W3D_Double Zclear=0.90;

/* lock hardware then clear */
	W3D_FlushFrame(context);
	W3D_WaitIdle(context);
	W3D_SetDrawRegion(context,bm,0,&scissor);
	WaitBlit();
	BltBitMapRastPort(bufferrastport.BitMap,0,0,window->RPort,0,0,LARGE,HIGH,0xC0);	/* copy the "back buffer" to the window */

	if( W3D_SUCCESS != W3D_LockHardware(context) ) return;

	W3D_ClearDrawRegion(context,0);
	W3D_ClearZBuffer(context, &Zclear);

	W3D_UnLockHardware(context);
}
/*==================================================================*/
void State( ULONG state)
{
UBYTE statename[50];

	statename[0]=0;
	if(state==W3D_AUTOTEXMANAGEMENT)	strcpy(statename,"W3D_AUTOTEXMANAGEMENT");
	if(state==W3D_SYNCHRON)			strcpy(statename,"W3D_SYNCHRON");
	if(state==W3D_INDIRECT)			strcpy(statename,"W3D_INDIRECT");
	if(state==W3D_GLOBALTEXENV)		strcpy(statename,"W3D_GLOBALTEXENV");
	if(state==W3D_DOUBLEHEIGHT)		strcpy(statename,"W3D_DOUBLEHEIGHT");
	if(state==W3D_FAST)				strcpy(statename,"W3D_FAST");
	if(state==W3D_TEXMAPPING)			strcpy(statename,"W3D_TEXMAPPING");
	if(state==W3D_PERSPECTIVE)			strcpy(statename,"W3D_PERSPECTIVE");
	if(state==W3D_GOURAUD)			strcpy(statename,"W3D_GOURAUD");
	if(state==W3D_ZBUFFER)			strcpy(statename,"W3D_ZBUFFER");
	if(state==W3D_ZBUFFERUPDATE)		strcpy(statename,"W3D_ZBUFFERUPDATE");
	if(state==W3D_BLENDING)			strcpy(statename,"W3D_BLENDING");
	if(state==W3D_FOGGING)			strcpy(statename,"W3D_FOGGING");
	if(state==W3D_ANTI_POINT)			strcpy(statename,"W3D_ANTI_POINT");
	if(state==W3D_ANTI_LINE)			strcpy(statename,"W3D_ANTI_LINE");
	if(state==W3D_ANTI_POLYGON)		strcpy(statename,"W3D_ANTI_POLYGON");
	if(state==W3D_ANTI_FULLSCREEN)		strcpy(statename,"W3D_ANTI_FULLSCREEN");
	if(state==W3D_DITHERING)			strcpy(statename,"W3D_DITHERING");
	if(state==W3D_LOGICOP)			strcpy(statename,"W3D_LOGICOP");
	if(state==W3D_STENCILBUFFER)		strcpy(statename,"W3D_STENCILBUFFER");
	if(state==W3D_ALPHATEST)			strcpy(statename,"W3D_ALPHATEST");
	if(state==W3D_SPECULAR)			strcpy(statename,"W3D_SPECULAR");
	if(state==W3D_TEXMAPPING3D)		strcpy(statename,"W3D_TEXMAPPING3D");
	if(state==W3D_CHROMATEST)			strcpy(statename,"W3D_CHROMATEST");

	if(statename[0]==0) return;
	result = W3D_GetState(context,state);
	if (result == W3D_ENABLED) printf(" [x]"); else printf(" [ ]");
	printf(" %s\n",&statename[4]);
}
/*==================================================================*/
void MyQueryDriver(W3D_Driver *driver, ULONG query, ULONG destfmt)
{
UBYTE destname[50];
UBYTE queryname[50];

	if(destfmt==W3D_FMT_CLUT)	strcpy(destname,"W3D_FMT_CLUT");
	if(destfmt==W3D_FMT_R5G5B5)	strcpy(destname,"W3D_FMT_R5G5B5");
	if(destfmt==W3D_FMT_B5G5R5)	strcpy(destname,"W3D_FMT_B5G5R5");
	if(destfmt==W3D_FMT_R5G5B5PC)	strcpy(destname,"W3D_FMT_R5G5B5PC");
	if(destfmt==W3D_FMT_B5G5R5PC)	strcpy(destname,"W3D_FMT_B5G5R5PC");
	if(destfmt==W3D_FMT_R5G6B5)	strcpy(destname,"W3D_FMT_R5G6B5");
	if(destfmt==W3D_FMT_B5G6R5)	strcpy(destname,"W3D_FMT_B5G6R5");
	if(destfmt==W3D_FMT_R5G6B5PC)	strcpy(destname,"W3D_FMT_R5G6B5PC");
	if(destfmt==W3D_FMT_B5G6R5PC)	strcpy(destname,"W3D_FMT_B5G6R5PC");
	if(destfmt==W3D_FMT_R8G8B8)	strcpy(destname,"W3D_FMT_R8G8B8");
	if(destfmt==W3D_FMT_B8G8R8)	strcpy(destname,"W3D_FMT_B8G8R8");
	if(destfmt==W3D_FMT_A8B8G8R8)	strcpy(destname,"W3D_FMT_A8B8G8R8");
	if(destfmt==W3D_FMT_A8R8G8B8)	strcpy(destname,"W3D_FMT_A8R8G8B8");
	if(destfmt==W3D_FMT_B8G8R8A8)	strcpy(destname,"W3D_FMT_B8G8R8A8");
	if(destfmt==W3D_FMT_R8G8B8A8)	strcpy(destname,"W3D_FMT_R8G8B8A8");

	queryname[0]=0;
	if(query==W3D_Q_DRAW_POINT)		strcpy(queryname,"W3D_Q_DRAW_POINT");
	if(query==W3D_Q_DRAW_LINE)		strcpy(queryname,"W3D_Q_DRAW_LINE");
	if(query==W3D_Q_DRAW_TRIANGLE)	strcpy(queryname,"W3D_Q_DRAW_TRIANGLE");
	if(query==W3D_Q_DRAW_POINT_X)		strcpy(queryname,"W3D_Q_DRAW_POINT_X");
	if(query==W3D_Q_DRAW_LINE_X)		strcpy(queryname,"W3D_Q_DRAW_LINE_X");
	if(query==W3D_Q_DRAW_LINE_ST)		strcpy(queryname,"W3D_Q_DRAW_LINE_ST");
	if(query==W3D_Q_DRAW_POLY_ST)		strcpy(queryname,"W3D_Q_DRAW_POLY_ST");
	if(query==W3D_Q_TEXMAPPING)		strcpy(queryname,"W3D_Q_TEXMAPPING");
	if(query==W3D_Q_MIPMAPPING)		strcpy(queryname,"W3D_Q_MIPMAPPING");
	if(query==W3D_Q_BILINEARFILTER)	strcpy(queryname,"W3D_Q_BILINEARFILTER");
	if(query==W3D_Q_MMFILTER)		strcpy(queryname,"W3D_Q_MMFILTER");
	if(query==W3D_Q_LINEAR_REPEAT)	strcpy(queryname,"W3D_Q_LINEAR_REPEAT");
	if(query==W3D_Q_LINEAR_CLAMP)		strcpy(queryname,"W3D_Q_LINEAR_CLAMP");
	if(query==W3D_Q_PERSPECTIVE)		strcpy(queryname,"W3D_Q_PERSPECTIVE");
	if(query==W3D_Q_PERSP_REPEAT)		strcpy(queryname,"W3D_Q_PERSP_REPEAT");
	if(query==W3D_Q_PERSP_CLAMP)		strcpy(queryname,"W3D_Q_PERSP_CLAMP");
	if(query==W3D_Q_ENV_REPLACE)		strcpy(queryname,"W3D_Q_ENV_REPLACE");
	if(query==W3D_Q_ENV_DECAL)		strcpy(queryname,"W3D_Q_ENV_DECAL");
	if(query==W3D_Q_ENV_MODULATE)		strcpy(queryname,"W3D_Q_ENV_MODULATE");
	if(query==W3D_Q_ENV_BLEND)		strcpy(queryname,"W3D_Q_ENV_BLEND");
	if(query==W3D_Q_FLATSHADING)		strcpy(queryname,"W3D_Q_FLATSHADING");
	if(query==W3D_Q_GOURAUDSHADING)	strcpy(queryname,"W3D_Q_GOURAUDSHADING");
	if(query==W3D_Q_ZBUFFER)		strcpy(queryname,"W3D_Q_ZBUFFER");
	if(query==W3D_Q_ZBUFFERUPDATE)	strcpy(queryname,"W3D_Q_ZBUFFERUPDATE");
	if(query==W3D_Q_ZCOMPAREMODES)	strcpy(queryname,"W3D_Q_ZCOMPAREMODES");
	if(query==W3D_Q_ALPHATEST)		strcpy(queryname,"W3D_Q_ALPHATEST");
	if(query==W3D_Q_ALPHATESTMODES)	strcpy(queryname,"W3D_Q_ALPHATESTMODES");
	if(query==W3D_Q_BLENDING)		strcpy(queryname,"W3D_Q_BLENDING");
	if(query==W3D_Q_SRCFACTORS)		strcpy(queryname,"W3D_Q_SRCFACTORS");
	if(query==W3D_Q_DESTFACTORS)		strcpy(queryname,"W3D_Q_DESTFACTORS");
	if(query==W3D_Q_FOGGING)		strcpy(queryname,"W3D_Q_FOGGING");
	if(query==W3D_Q_LINEAR)			strcpy(queryname,"W3D_Q_LINEAR");
	if(query==W3D_Q_EXPONENTIAL)		strcpy(queryname,"W3D_Q_EXPONENTIAL");
	if(query==W3D_Q_S_EXPONENTIAL)	strcpy(queryname,"W3D_Q_S_EXPONENTIAL");
	if(query==W3D_Q_ANTIALIASING)		strcpy(queryname,"W3D_Q_ANTIALIASING");
	if(query==W3D_Q_ANTI_POINT)		strcpy(queryname,"W3D_Q_ANTI_POINT");
	if(query==W3D_Q_ANTI_LINE)		strcpy(queryname,"W3D_Q_ANTI_LINE");
	if(query==W3D_Q_ANTI_POLYGON)		strcpy(queryname,"W3D_Q_ANTI_POLYGON");
	if(query==W3D_Q_ANTI_FULLSCREEN)	strcpy(queryname,"W3D_Q_ANTI_FULLSCREEN");
	if(query==W3D_Q_DITHERING)		strcpy(queryname,"W3D_Q_DITHERING");
	if(query==W3D_Q_SCISSOR)		strcpy(queryname,"W3D_Q_SCISSOR");
	if(query==W3D_Q_RECTTEXTURES)		strcpy(queryname,"W3D_Q_RECTTEXTURES");
	if(query==W3D_Q_LOGICOP)		strcpy(queryname,"W3D_Q_LOGICOP");
	if(query==W3D_Q_MASKING)		strcpy(queryname,"W3D_Q_MASKING");
	if(query==W3D_Q_STENCILBUFFER)	strcpy(queryname,"W3D_Q_STENCILBUFFER");
	if(query==W3D_Q_STENCIL_MASK)		strcpy(queryname,"W3D_Q_STENCIL_MASK");
	if(query==W3D_Q_STENCIL_FUNC)		strcpy(queryname,"W3D_Q_STENCIL_FUNC");
	if(query==W3D_Q_STENCIL_SFAIL)	strcpy(queryname,"W3D_Q_STENCIL_SFAIL");
	if(query==W3D_Q_STENCIL_DPFAIL)	strcpy(queryname,"W3D_Q_STENCIL_DPFAIL");
	if(query==W3D_Q_STENCIL_DPPASS)	strcpy(queryname,"W3D_Q_STENCIL_DPPASS");
	if(query==W3D_Q_STENCIL_WRMASK)	strcpy(queryname,"W3D_Q_STENCIL_WRMASK");
	if(query==W3D_Q_PALETTECONV)		strcpy(queryname,"W3D_Q_PALETTECONV");
	if(query==W3D_Q_DRAW_POINT_FX)	strcpy(queryname,"W3D_Q_DRAW_POINT_FX");
	if(query==W3D_Q_DRAW_POINT_TEX)	strcpy(queryname,"W3D_Q_DRAW_POINT_TEX");
	if(query==W3D_Q_DRAW_LINE_FX)		strcpy(queryname,"W3D_Q_DRAW_LINE_FX");
	if(query==W3D_Q_DRAW_LINE_TEX)	strcpy(queryname,"W3D_Q_DRAW_LINE_TEX");
	if(query==W3D_Q_SPECULAR)		strcpy(queryname,"W3D_Q_SPECULAR");
	if(query==W3D_Q_CULLFACE)		strcpy(queryname,"W3D_Q_CULLFACE");

	if(queryname[0]==0) return;
	result = W3D_QueryDriver(driver,query,destfmt);
	if (result != W3D_NOT_SUPPORTED) printf(" MyQueryDriver[x]"); else printf(" MyQueryDriver[ ]");
	result = W3D_Query(context,query,destfmt);
	if (result != W3D_NOT_SUPPORTED) printf(" Query[x]"); else printf(" Query[ ]");
	printf(" %s for destformat %s \n",&queryname[6],&destname[8]);
}
/*==================================================================*/
void QueryTex(ULONG texfmt,ULONG destfmt)
{
UBYTE destname[50];
UBYTE texname[50];

	if(destfmt==W3D_FMT_CLUT)	strcpy(destname,"W3D_FMT_CLUT");
	if(destfmt==W3D_FMT_R5G5B5)	strcpy(destname,"W3D_FMT_R5G5B5");
	if(destfmt==W3D_FMT_B5G5R5)	strcpy(destname,"W3D_FMT_B5G5R5");
	if(destfmt==W3D_FMT_R5G5B5PC)	strcpy(destname,"W3D_FMT_R5G5B5PC");
	if(destfmt==W3D_FMT_B5G5R5PC)	strcpy(destname,"W3D_FMT_B5G5R5PC");
	if(destfmt==W3D_FMT_R5G6B5)	strcpy(destname,"W3D_FMT_R5G6B5");
	if(destfmt==W3D_FMT_B5G6R5)	strcpy(destname,"W3D_FMT_B5G6R5");
	if(destfmt==W3D_FMT_R5G6B5PC)	strcpy(destname,"W3D_FMT_R5G6B5PC");
	if(destfmt==W3D_FMT_B5G6R5PC)	strcpy(destname,"W3D_FMT_B5G6R5PC");
	if(destfmt==W3D_FMT_R8G8B8)	strcpy(destname,"W3D_FMT_R8G8B8");
	if(destfmt==W3D_FMT_B8G8R8)	strcpy(destname,"W3D_FMT_B8G8R8");
	if(destfmt==W3D_FMT_A8B8G8R8)	strcpy(destname,"W3D_FMT_A8B8G8R8");
	if(destfmt==W3D_FMT_A8R8G8B8)	strcpy(destname,"W3D_FMT_A8R8G8B8");
	if(destfmt==W3D_FMT_B8G8R8A8)	strcpy(destname,"W3D_FMT_B8G8R8A8");
	if(destfmt==W3D_FMT_R8G8B8A8)	strcpy(destname,"W3D_FMT_R8G8B8A8");

	if(texfmt==W3D_CHUNKY)		strcpy(texname,"W3D_CHUNKY  ");
	if(texfmt==W3D_A1R5G5B5)	strcpy(texname,"W3D_A1R5G5B5");
	if(texfmt==W3D_R5G6B5)		strcpy(texname,"W3D_R5G6B5  ");
	if(texfmt==W3D_R8G8B8)		strcpy(texname,"W3D_R8G8B8  ");
	if(texfmt==W3D_A4R4G4B4)	strcpy(texname,"W3D_A4R4G4B4");
	if(texfmt==W3D_A8R8G8B8)	strcpy(texname,"W3D_A8R8G8B8");
	if(texfmt==W3D_A8)		strcpy(texname,"W3D_A8      ");
	if(texfmt==W3D_L8)		strcpy(texname,"W3D_L8      ");
	if(texfmt==W3D_L8A8)		strcpy(texname,"W3D_L8A8    ");
	if(texfmt==W3D_I8)		strcpy(texname,"W3D_I8      ");
	if(texfmt==W3D_R8G8B8A8)	strcpy(texname,"W3D_R8G8B8A8");

	printf("  Hardware Support for texfmt %s desfmt %s \t",&texname[4],&destname[8]);
	result = W3D_GetTexFmtInfo(context, texfmt, destfmt);
	if (result & W3D_TEXFMT_FAST)
	printf("[FAST]"); else printf("[....]");
	if (result & W3D_TEXFMT_CLUTFAST)
	printf("[CLUTFAST]"); else printf("[........]");
	if (result & W3D_TEXFMT_ARGBFAST)
	printf("[ARGB]"); else printf("[....]");
	if (result & W3D_TEXFMT_UNSUPPORTED)
	printf("[NO]"); else printf("[..]");
	if (result & W3D_TEXFMT_SUPPORTED)
	printf("[SUP]"); else printf("[...]");
	printf("\n");
}
/*==================================================================*/
ULONG SetBlendMode()
{
	if(srcfunc==0)
	if(dstfunc==0)
		return(W3D_ILLEGALINPUT);

	if((srcfunc==W3D_ZERO) && (dstfunc==W3D_ZERO))	/* skip this special value that is used in Wazp3D */
		return(W3D_SUCCESS);

	return(W3D_SetBlendMode(context,srcfunc,dstfunc));
}
/*==================================================================*/
void CheckWarp3D(void)
{
W3D_Driver **drivers;
W3D_Driver *driver=NULL;
ULONG texfmt,destfmt,state,query,w,h,wp,hp,m,n;

	printf("CheckWarp3D:\n");
	printf("==============================\n");

	flags = W3D_CheckDriver();
	if (flags & W3D_DRIVER_3DHW)		{printf("Hardware W3D_Driver available\n");}
	if (flags & W3D_DRIVER_CPU)		{printf("Software W3D_Driver available\n");}
	if (flags & W3D_DRIVER_UNAVAILABLE) {printf("No W3D_Driver !!!\n");return;}

	drivers = W3D_GetDrivers();
	if (*drivers == NULL)
		return;

	printf("==============================\n");
	while (drivers[0])
	{
	driver=drivers[0];
	printf("========= W3D_Driver <%s> soft:%d ChipID:%ld formats:%ld =======\n",driver->name,driver->swdriver,(long)driver->ChipID,(long)driver->formats);

	MLOOP(15)
	{
	destfmt=1<<m;
	printf("------------------------------\n");
	printf("destformat:%ld\n",(long)destfmt);
	MyQueryDriver(driver,W3D_Q_DRAW_POINT,destfmt);
	MyQueryDriver(driver,W3D_Q_DRAW_LINE,destfmt);
	MyQueryDriver(driver,W3D_Q_DRAW_TRIANGLE,destfmt);
	w =W3D_QueryDriver(driver,W3D_Q_MAXTEXWIDTH   ,destfmt);
	h =W3D_QueryDriver(driver,W3D_Q_MAXTEXHEIGHT  ,destfmt);
	wp=W3D_QueryDriver(driver,W3D_Q_MAXTEXWIDTH_P ,destfmt);
	hp=W3D_QueryDriver(driver,W3D_Q_MAXTEXHEIGHT_P,destfmt);
	printf("Max texture size %ld X %ld (perspective %ld X %ld )\n",(long)w,(long)h,(long)wp,(long)hp);
	}

	drivers++;
	}

/* recover current bitmap's destfmt */
	destfmt=GetCyberMapAttr(bm,CYBRMATTR_PIXFMT);
	if(destfmt==PIXFMT_LUT8)	destfmt=W3D_FMT_CLUT;
	if(destfmt==PIXFMT_RGB15)	destfmt=W3D_FMT_R5G5B5;
	if(destfmt==PIXFMT_BGR15)	destfmt=W3D_FMT_B5G5R5;
	if(destfmt==PIXFMT_RGB15PC)	destfmt=W3D_FMT_R5G5B5PC;
	if(destfmt==PIXFMT_BGR15PC)	destfmt=W3D_FMT_B5G5R5PC;
	if(destfmt==PIXFMT_RGB16)	destfmt=W3D_FMT_R5G6B5;
	if(destfmt==PIXFMT_BGR16)	destfmt=W3D_FMT_B5G6R5;
	if(destfmt==PIXFMT_RGB16PC)	destfmt=W3D_FMT_R5G6B5PC;
	if(destfmt==PIXFMT_BGR16PC)	destfmt=W3D_FMT_B5G6R5PC;
	if(destfmt==PIXFMT_RGB24)	destfmt=W3D_FMT_R8G8B8;
	if(destfmt==PIXFMT_BGR24)	destfmt=W3D_FMT_B8G8R8;
	if(destfmt==PIXFMT_ARGB32)	destfmt=W3D_FMT_A8R8G8B8;
	if(destfmt==PIXFMT_BGRA32)	destfmt=W3D_FMT_B8G8R8A8;
	if(destfmt==PIXFMT_RGBA32)	destfmt=W3D_FMT_R8G8B8A8;
	printf("==============================\n");
	printf("Current bitmap's destformat is %ld\n",(long)destfmt);

	printf("==============================\n");
	printf("Query for the current bitmap's destformat:\n");
	NLOOP(162)
	{
	query=n+1;
	if(query!=W3D_Q_MAXTEXWIDTH)
	if(query!=W3D_Q_MAXTEXHEIGHT)
	if(query!=W3D_Q_MAXTEXWIDTH_P)
	if(query!=W3D_Q_MAXTEXHEIGHT_P)
		MyQueryDriver(driver,query,destfmt);
	}

	printf("==============================\n");
	printf("State default values:\n");
	NLOOP(27)
	{
	state=1<<(n+1);
		State(state);
	}


	printf("==============================\n");
	printf("Textures formats/bitmaps destformats: \n");
	MLOOP(11)
	{
	texfmt=m+1;
		NLOOP(15)
		{
		destfmt=1<<n;
		QueryTex(texfmt,destfmt);
		}
	printf("==============================\n");

	}

	MLOOP(15)
	{
	srcfunc=m+1; 	/* src/dstfunc go 1 to 15 */
	if(srcfunc<10) printf("BlendMode Src%ld :",(long)srcfunc); else printf("BlendMode Src%ld:",(long)srcfunc);
	NLOOP(15)
	{
	dstfunc=n+1;

	result=SetBlendMode();
	if(result==W3D_SUCCESS)
		{if(dstfunc<10) printf(" Dst%ld ",(long)dstfunc); else printf(" Dst%ld",(long)dstfunc);}
	else
		{if(dstfunc<10) printf(" ---- "); else printf(" -----");}
	}
	printf("\n");
	}
	srcfunc=dstfunc=0;
	printf("==============================\n");

}
/*=================================================================*/
BOOL StartWarp3D(void)
{						/* open a window & a ratsport ("back buffer") & open warp3d & create a warp3d context  */
UWORD screenlarge,screenhigh;
ULONG Flags =WFLG_ACTIVATE | WFLG_REPORTMOUSE | WFLG_RMBTRAP | WFLG_SIMPLE_REFRESH | WFLG_GIMMEZEROZERO ;
ULONG IDCMPs=IDCMP_CLOSEWINDOW | IDCMP_VANILLAKEY | IDCMP_RAWKEY | IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS ;
#ifdef STATWAZP3D
W3D_Driver **drivers;
#endif

	CyberGfxBase = OpenLibrary("cybergraphics.library", 0L);
	if (CyberGfxBase==NULL)
		{printf("Cant open LIBS:cybergraphics.library\n");	return FALSE;};

#ifdef STATWAZP3D
	WAZP3D_Init(SysBase);
#else
	Warp3DBase = OpenLibrary("Warp3D.library", 4L);
	if (!Warp3DBase)
		{printf("Cant open LIBS:Warp3D.library\n");	return FALSE;};
#endif

	screen 	= LockPubScreen("Workbench") ;
	screenlarge	=screen->Width; 	screenhigh	=screen->Height;
	ModeID = GetVPModeID(&screen->ViewPort);
	UnlockPubScreen(NULL, screen);

	window = OpenWindowTags(NULL,
	WA_Activate,	TRUE,
	WA_InnerWidth,	LARGE,
	WA_InnerHeight,	HIGH,
	WA_Left,		(screenlarge - LARGE)/2,
	WA_Top,		(screenhigh  -  HIGH)/2,
	WA_Title,		(IPTR)progname,
	WA_DragBar,		TRUE,
	WA_CloseGadget,	TRUE,
	WA_GimmeZeroZero,	TRUE,
	WA_Backdrop,	FALSE,
	WA_Borderless,	FALSE,
	WA_IDCMP,		IDCMPs,
	WA_Flags,		Flags,
	TAG_DONE);

	if (window==NULL)
		{printf("Cant open window\n");return FALSE;}

	InitRastPort( &bufferrastport );				/* allocate an other bitmap/rastport four double buffering */
	ScreenBits  = GetBitMapAttr( window->WScreen->RastPort.BitMap, BMA_DEPTH );
	flags = BMF_DISPLAYABLE|BMF_MINPLANES;
	bufferrastport.BitMap = AllocBitMap(LARGE,HIGH,ScreenBits, flags, window->RPort->BitMap);
	if(bufferrastport.BitMap==NULL)
		{printf("No Bitmap\n");return FALSE;}

	bm=bufferrastport.BitMap;				/* draw in this back-buffer */

#ifdef STATWAZP3D
#define W3D_Q_SETTINGS 9999
	drivers = W3D_GetDrivers();
	if (*drivers == NULL)
		{printf("No Warp3D driver found !!!\n");return(FALSE);}
	W3D_QueryDriver(drivers[0],W3D_Q_SETTINGS,0);
#endif
	context = W3D_CreateContextTags(&result,
		W3D_CC_MODEID,      ModeID,             // Mandatory for non-pubscreen
		W3D_CC_BITMAP,      (IPTR)bm,           // The bitmap we'll use
		W3D_CC_YOFFSET,     0,                  // We don't do dbuffering
		W3D_CC_DRIVERTYPE,  W3D_DRIVER_BEST,    // Let Warp3D decide
		W3D_CC_DOUBLEHEIGHT,FALSE,               // Double height screen
		W3D_CC_FAST,        TRUE,               // Fast drawing
	TAG_DONE);

	if (!context || result != W3D_SUCCESS)
		{printf("Cant create Warp3D context! (error %ld)\n",(long)result);return(FALSE);}

	result=W3D_AllocZBuffer(context);
	if(result!=W3D_SUCCESS)
		{printf("Cant create zbuffer! (error %ld)\n",(long)result);return(FALSE);}
	return(TRUE);
}
/*=================================================================*/
void CloseWarp3D(void)
{
	if (BufferedP)			free(BufferedP);

	if (CowObj)
	{
		if (CowObj->tex)			W3D_FreeTexObj(context,CowObj->tex);
		if (CowObj->picture)		free(CowObj->picture);
		free(CowObj);
	}

	if (CosmosObj)
	{
		if (CosmosObj->tex)		W3D_FreeTexObj(context,CosmosObj->tex);
		if (CosmosObj->picture)		free(CosmosObj->picture);
		free(CosmosObj);
	}

	if (QuadObj)			free(QuadObj);

	if (context)			W3D_DestroyContext(context);
	if (bufferrastport.BitMap)	FreeBitMap(bufferrastport.BitMap);
	if (window)				CloseWindow(window);

#ifdef STATWAZP3D
	WAZP3D_Close();
#else
	if (Warp3DBase)			CloseLibrary(Warp3DBase);
#endif

}
/*=================================================================*/
void SetStatesWarp3D(void)
{
W3D_Color envcolor1;
W3D_Color envcolor2;

	envcolor1.r=0.0; envcolor1.g=1.0; envcolor1.b=0.0; envcolor1.a=1.0;
	envcolor2.r=0.0; envcolor2.g=0.0; envcolor2.b=1.0; envcolor2.a=1.0;

	W3D_SetState(context, W3D_BLENDING,		W3D_DISABLE);	/* non transparent */
	W3D_SetState(context, W3D_GOURAUD,		W3D_DISABLE);	/* non shaded */
	W3D_SetState(context, W3D_PERSPECTIVE,	W3D_DISABLE);	/* not needed here */

	W3D_SetState(context, W3D_TEXMAPPING,	W3D_ENABLE);	/* use textures */
	W3D_SetState(context, W3D_ZBUFFER,		W3D_ENABLE);
	W3D_SetState(context, W3D_ZBUFFERUPDATE,	W3D_ENABLE);
	W3D_SetZCompareMode(context,zmode);					/* use zbuffer = remove hidden pixels */
	W3D_SetState(context, W3D_SCISSOR,		W3D_ENABLE);	/* clip to screen size */
	W3D_SetFrontFace(context,W3D_CCW);
	W3D_SetState(context, W3D_CULLFACE,		W3D_ENABLE);	/* remove hidden faces */

	W3D_SetState(context, W3D_GOURAUD,		W3D_ENABLE);	/* patch: gouraud is needed on some hardware */

	if(!hideface)
		W3D_SetState(context,W3D_CULLFACE,		W3D_DISABLE);
	if(!zbuffer)
		W3D_SetState(context, W3D_ZBUFFER,		W3D_DISABLE);
	if(!zupdate)
		W3D_SetState(context, W3D_ZBUFFERUPDATE,	W3D_DISABLE);
	if(!colored)
		W3D_SetState(context, W3D_TEXMAPPING,	W3D_DISABLE);

	W3D_SetTexEnv(context,CowObj->tex,texenvmode,&envcolor1);
	W3D_SetTexEnv(context,CosmosObj->tex,texenvmode,&envcolor2);

	SetBlendMode();
}
/*================================================================================*/
void ShowSrcDst(void)
{
#define  WINFO(var,val,doc) if(var == val) printf(" " #var "=" #val ", " #doc "\n");

	printf("Blending src%ld/dst%ld\n",(long)srcfunc,(long)dstfunc);

	WINFO(srcfunc,W3D_ZERO,"source + dest ")
	WINFO(srcfunc,W3D_ONE,"source + dest ")
	WINFO(srcfunc,W3D_SRC_COLOR,"dest only !!!!")
	WINFO(srcfunc,W3D_DST_COLOR,"source only ")
	WINFO(srcfunc,W3D_ONE_MINUS_SRC_COLOR,"dest only !!!!")
	WINFO(srcfunc,W3D_ONE_MINUS_DST_COLOR,"source only ")
	WINFO(srcfunc,W3D_SRC_ALPHA,"source + dest ")
	WINFO(srcfunc,W3D_ONE_MINUS_SRC_ALPHA,"source + dest ")
	WINFO(srcfunc,W3D_DST_ALPHA,"source + dest ")
	WINFO(srcfunc,W3D_ONE_MINUS_DST_ALPHA,"source + dest ")
	WINFO(srcfunc,W3D_SRC_ALPHA_SATURATE,"source only ")
	WINFO(srcfunc,W3D_CONSTANT_COLOR," ");
	WINFO(srcfunc,W3D_ONE_MINUS_CONSTANT_COLOR," ");
	WINFO(srcfunc,W3D_CONSTANT_ALPHA," ");
	WINFO(srcfunc,W3D_ONE_MINUS_CONSTANT_ALPHA," ");

	WINFO(dstfunc,W3D_ZERO,"source + dest ")
	WINFO(dstfunc,W3D_ONE,"source + dest ")
	WINFO(dstfunc,W3D_SRC_COLOR,"dest only ")
	WINFO(dstfunc,W3D_DST_COLOR,"source only !!!!")
	WINFO(dstfunc,W3D_ONE_MINUS_SRC_COLOR,"dest only ")
	WINFO(dstfunc,W3D_ONE_MINUS_DST_COLOR,"source only !!!!")
	WINFO(dstfunc,W3D_SRC_ALPHA,"source + dest ")
	WINFO(dstfunc,W3D_ONE_MINUS_SRC_ALPHA,"source + dest ")
	WINFO(dstfunc,W3D_DST_ALPHA,"source + dest ")
	WINFO(dstfunc,W3D_ONE_MINUS_DST_ALPHA,"source + dest ")
	WINFO(dstfunc,W3D_SRC_ALPHA_SATURATE,"source only !!!!")
	WINFO(dstfunc,W3D_CONSTANT_COLOR," ");
	WINFO(dstfunc,W3D_ONE_MINUS_CONSTANT_COLOR," ");
	WINFO(dstfunc,W3D_CONSTANT_ALPHA," ");
	WINFO(dstfunc,W3D_ONE_MINUS_CONSTANT_ALPHA," ");

/* check if this blend mode works ? */
	result=SetBlendMode();
	if(result!=W3D_SUCCESS)
		printf("Cant SetBlendMode!(src %ld dst %ld)\n",(long)srcfunc,(long)dstfunc);
}
/*================================================================================*/
void WindowEvents(void)
{							/* manage the window  */
struct IntuiMessage *imsg;

	while( (imsg = (struct IntuiMessage *)GetMsg(window->UserPort)))
	{
	if (imsg == NULL) break;
	switch (imsg->Class)
		{
		case IDCMP_CLOSEWINDOW:
			closed=TRUE;				break;
		case IDCMP_VANILLAKEY:
			switch(imsg->Code)
			{
			case 'f':	showfps=!showfps;		break;
			case 'o':	optimroty=!optimroty;	break;

			case 'z':	zbuffer=!zbuffer;		break;
			case '1': 	zmode=W3D_Z_NEVER; 	break;
			case '2': 	zmode=W3D_Z_LESS; 	break;
			case '3': 	zmode=W3D_Z_GEQUAL; 	break;
			case '4': 	zmode=W3D_Z_LEQUAL; 	break;
			case '5': 	zmode=W3D_Z_GREATER; 	break;
			case '6': 	zmode=W3D_Z_NOTEQUAL; 	break;
			case '7': 	zmode=W3D_Z_EQUAL; 	break;
			case '8': 	zmode=W3D_Z_ALWAYS; 	break;
			case 'u':	zupdate=!zupdate;		break;
			case 'e':	drawmode='e';		break;
			case 't':	drawmode='t';		break;
			case 'l':	drawmode='l';		break;
			case 'p':	drawmode='p';		break;
			case '0':	bigpoint=!bigpoint;	break;
			case 'c':	colored=!colored;		break;
			case 'C':	drawcow=!drawcow;		break;
			case 'h':	hideface=!hideface;	break;
			case 'g':	greyreadz=!greyreadz;	break;
			case 'r':	rotate=!rotate;		break;
			case 'P':	lintest=!lintest;		break;
			case 'L':	lintest=!lintest;		break;
			case 'Z':	ztest=!ztest;		break;
			case 'b':	buffered=!buffered;	break;

			case 'S':
				srcdstfunc--;
				if(srcdstfunc>255) srcdstfunc=0;
				srcfunc=srcdstfunc/16;
				dstfunc=srcdstfunc-srcfunc*16;
				ShowSrcDst();
				break;
			case 'D':
				srcdstfunc++;
				if(srcdstfunc>255) srcdstfunc=0;
				srcfunc=srcdstfunc/16;
				dstfunc=srcdstfunc-srcfunc*16;
				ShowSrcDst();
				break;

			case 's':
				srcfunc++;
				if(srcfunc>15) 	srcfunc=0;
				srcdstfunc=srcfunc*16+dstfunc;
				ShowSrcDst();
				break;
			case 'd':
				dstfunc++;
				if(dstfunc>15) 	dstfunc=0;
				srcdstfunc=srcfunc*16+dstfunc;
				ShowSrcDst();
				break;

			case 'm':
			case 'M':
				texenvmode++;	if(texenvmode>W3D_BLEND)	texenvmode=W3D_REPLACE;
				printf("Texenvmode %ld\n",(long)texenvmode);
				WINFO(texenvmode,W3D_REPLACE,"unlit texturing ")
				WINFO(texenvmode,W3D_DECAL,"same as W3D_REPLACE use alpha to blend texture with primitive =lit-texturing")
				WINFO(texenvmode,W3D_MODULATE,"lit-texturing by modulation ")
				WINFO(texenvmode,W3D_BLEND,"blend with environment color ")
				break;

			case 'w':
			w3dpatch=!w3dpatch;
				if(w3dpatch)	printf("Warp3D is  patched\n");
				else			printf("Warp3D not patched\n");
				break;

			case 27:	closed=TRUE;		break;
			default:					break;
			}
			break;
		default:
			break;
		}
	if(imsg)
		{ReplyMsg((struct Message *)imsg);imsg = NULL;}
	}
}
/*=================================================================*/
int main(int argc, char *argv[])
{
ULONG frame,n;
BOOL framebuffered;

	FpsText[0]=zname[0]=0;
	if(!StartWarp3D())
		goto panic;

	CheckWarp3D();

	QuadObj=AddObject(4,2*3);		/* simple quad as 2 tris */

	CosmosObj=AddObject(4,2*3);		/* simple quad as 2 tris */
	SetQuad(CosmosObj,0.0,0.0,0.01,LARGE,HIGH,255,200,20,255);
	LoadTextureWarp3D(CosmosObj,"Cosmos_256X256X32.RAW",256,32);	/* texture data as R8 G8 B8 A8*/

	CowObj =AddObject(pointsCount,trianglesCount*3);
	ReadIncludedObject(CowObj,points,indices);			/* read data in Cow3D.h */
	LoadTextureWarp3D(CowObj,TEXNAME,TEXSIZE,32);			/* texture data as R8 G8 B8 A8*/

	while(!closed)
	{

		framebuffered=FALSE;

		if(buffered)
		if(BufferedP==NULL)
			{
			printf("Will allocate buffer %ld bytes\n",(long)(FRAMESCOUNT*CowObj->Pnb*sizeof(struct point3D)));
			BufferedP = malloc(FRAMESCOUNT*CowObj->Pnb*sizeof(struct point3D));
			if(!BufferedP) printf("no memory for buffer!\n");
			NLOOP(FRAMESCOUNT) Pdone[n]=FALSE;
			}

		SetStatesWarp3D();

		if(rotate)
		{
		frame=RotY/SPEED;

		if(buffered)
		if(BufferedP!=NULL)
		if(Pdone[frame])		/* use a saved frame ? */
			{
			CowObj->P2		=&BufferedP[CowObj->Pnb*frame];	/* P2=buffered points */
			framebuffered=TRUE;
			}

		if(!framebuffered)		/* then transform/rotate/project frame  */
			{
			CowObj->P2		=&CowObj->P[CowObj->Pnb];		/* use default P2 */
			memcpy(CowObj->P2,CowObj->P,CowObj->Pnb*sizeof(struct point3D));	/* copy the object's points to P2*/
			SetMry(ViewMatrix,RotY);						/* do a y rotation matrix */
			if(optimroty)
				YrotateP(ViewMatrix,CowObj->P2,CowObj->Pnb);		/* only y-rotate the object's points in P2*/
			else
				TransformP(ViewMatrix,CowObj->P2,CowObj->Pnb);		/* fully transform the object's points in P2*/
			ProjectP(CowObj->P2,CowObj->Pnb);					/* project points to screen */
			}

		if(buffered)
		if(BufferedP!=NULL)
		if(!Pdone[frame])		/* then save this frame */
			{
			memcpy(&BufferedP[CowObj->Pnb*frame],CowObj->P2,CowObj->Pnb*sizeof(struct point3D));	/* save the transformed points to buffer */
			Pdone[frame]=TRUE;
			}

		}

		W3D_SetState(context, W3D_BLENDING,W3D_DISABLE);

		if(drawcow)
			DrawObject(CowObj);						/* draw with warp3d */

		if(srcfunc!=0)
		if(dstfunc!=0)
			{
			W3D_SetState(context, W3D_BLENDING,W3D_ENABLE);
			DrawObject(CosmosObj);
			}

		if(lintest)
			DrawPointLineTestsWarp3D(CowObj->tex);
		if(ztest)
			DrawZtests();

		W3D_Flush(context);

		if(greyreadz)
			DrawZvaluesWarp3D();

		SwitchDisplayWarp3D();						/* copy to window */
		WindowEvents();							/* is window closed ? */
		if(showfps) DoFPS(); 						/* count the average "frames per second" */
		RotY=RotY+SPEED;							/* rotate */
		if(RotY>=360.0) RotY=0.0;					/* rotate */
	}
panic:
	CloseWarp3D();
	return 0;
}
/*=================================================================*/
