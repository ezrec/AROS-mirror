/* patches for Warp3D - Alain THELLIER - Paris - FRANCE 					*/
/* This file  patch some Warp3D bugged functions:						*/
/* patch for real Warp3D if the hardware-driver cant draw lines/points			*/
/* patch for real Warp3D if the hardware-driver cant read/write in zbuffer		*/

BOOL w3dpatch=FALSE;
/*==========================================================================*/
#define ZDOUBLE	1
#define ZDOUBLEPC	2
#define ZFLOAT	3
#define ZFLOATPC	4
#define ZWORD	5
#define ZWORDPC	6
#define ZUWORD	7
#define ZUWORDPC	8
#define Z24S8	9
#define Z24S8PC	10
#define S8Z24	11
#define S8Z24PC	12

static ULONG zsizes[13]={0,64,64,32,32,16,16,16,16,32,32,32,32};
static LONG zformat=0;
#define ILOOP(nbre) for(i=0;i<nbre;i++)
#define NLOOP(nbre) for(n=0;n<nbre;n++)
void findzformat(W3D_Context *context);
/*==========================================================================*/
/* this define a disc shape to draw the big points */
float discx[36]={0.5000,0.4924,0.4698,0.4330,0.3830,0.3214,0.2500,0.1710,0.0868,0.0000,-0.0868,-0.1710,-0.2500,-0.3214,-0.3830,-0.4330,-0.4698,-0.4924,-0.5000,-0.4924,-0.4698,-0.4330,-0.3830,-0.3214,-0.2500,-0.1710,-0.0868,0.0000,0.0868,0.1710,0.2500,0.3214,0.3830,0.4330,0.4698,0.4924};
float discy[36]={0.0000,0.0868,0.1710,0.2500,0.3214,0.3830,0.4330,0.4698,0.4924,0.5000,0.4924,0.4698,0.4330,0.3830,0.3214,0.2500,0.1710,0.0868,0.0000,-0.0868,-0.1710,-0.2500,-0.3214,-0.3830,-0.4330,-0.4698,-0.4924,-0.5000,-0.4924,-0.4698,-0.4330,-0.3830,-0.3214,-0.2500,-0.1710,-0.0868};
/*==========================================================================*/
ULONG DrawLine(W3D_Context *context,W3D_Vertex *v1,W3D_Vertex *v2,W3D_Texture *tex,float size)
{
/* draw a line as a quad with variable size */
W3D_Vertex quad[4];
W3D_Triangles triangles;
register ULONG result;
float dim,x,y;
ULONG currentstate;


	if(size<1.0) size=1.0;
	currentstate=W3D_GetState(context,W3D_CULLFACE);
	W3D_SetState(context,W3D_CULLFACE,W3D_DISABLE);		/* a line is never an hidden-face */
	triangles.v 		=quad;
	triangles.tex		=tex;
	triangles.vertexcount	=4;
	memcpy(&quad[0],v1,sizeof(W3D_Vertex));
	memcpy(&quad[1],v2,sizeof(W3D_Vertex));
	memcpy(&quad[2],v2,sizeof(W3D_Vertex));
	memcpy(&quad[3],v1,sizeof(W3D_Vertex));
	x=(v2->x - v1->x);
	y=(v2->y - v1->y);
	dim=sqrt( x*x + y*y );

	if(dim!=0.0)
	{
	size=size/(dim*2.0);
	x=size*x;
	y=size*y;
	quad[0].x += y; 	quad[0].y -= x;
 	quad[1].x += y; 	quad[1].y -= x; 
	quad[2].x -= y;	quad[2].y += x;
	quad[3].x -= y;	quad[3].y += x;
	}
	else
	{
	quad[0].x += 0.0; 	quad[0].y += 0.0;
 	quad[1].x += size; 	quad[1].y += 0.0; 
 	quad[2].x += size; 	quad[2].y += size; 
 	quad[3].x += 0.0; 	quad[3].y += size; 
	}

	result=W3D_DrawTriFan(context,&triangles);
	W3D_SetState(context,W3D_CULLFACE,currentstate);
	return(result);
}
/*==========================================================================*/
ULONG DrawDisc(W3D_Context *context,W3D_Vertex *v1,W3D_Texture *tex,float size)
{
/* draw a disc as a trifan with variable size */
W3D_Vertex disc[36];
W3D_Triangles triangles;
register W3D_Vertex *v=disc;
register ULONG result;
register ULONG n;
ULONG currentstate;

	currentstate=W3D_GetState(context,W3D_CULLFACE);
	W3D_SetState(context,W3D_CULLFACE,W3D_DISABLE);		/* a point is never an hidden-face */
	triangles.v 		=v;
	triangles.tex		=tex;
	triangles.vertexcount	=36;
	NLOOP(36)
		{
		memcpy(v,v1,sizeof(W3D_Vertex));
		v->x += discx[n]*size;
		v->y += discy[n]*size;
		v++;
		}
	result=W3D_DrawTriFan(context,&triangles);
	W3D_SetState(context,W3D_CULLFACE,currentstate);
	return(result);
}
/*==========================================================================*/
ULONG PatchW3D_DrawLine(W3D_Context *context, W3D_Line *line)
{
/* draw a single line */
register ULONG result;

	if(!w3dpatch)	return( W3D_DrawLine(context,line) );

	result=DrawLine(context,&line->v1,&line->v2,line->tex,line->linewidth);
	return(result);
}
/*==========================================================================*/
ULONG PatchW3D_DrawLineStrip(W3D_Context *context, W3D_Lines *lines)
{
register W3D_Vertex *v	=lines->v;
register LONG nlines	=lines->vertexcount-1;
register ULONG result	=W3D_SUCCESS;
register LONG n;

	if(!w3dpatch)	return( W3D_DrawLineStrip(context,lines) );
	
NLOOP(nlines)
	{
	result=DrawLine(context,&v[0],&v[1],lines->tex,lines->linewidth);
	v++;
	}
	return(result);
}
/*==========================================================================*/
ULONG PatchW3D_DrawLineLoop(W3D_Context *context, W3D_Lines *lines)
{
register W3D_Vertex *v	=lines->v;
register LONG nlines	=lines->vertexcount-1;
register ULONG result;
register LONG n;

	if(!w3dpatch)	return( W3D_DrawLineLoop(context,lines) );

NLOOP(nlines)
	{
	result=DrawLine(context,&v[0],&v[1],lines->tex,lines->linewidth);
	v++;
	}
	result=DrawLine(context,&v[0],lines->v,lines->tex,lines->linewidth);
	return(result);
}
/*==========================================================================*/
ULONG PatchW3D_DrawPoint(W3D_Context *context, W3D_Point *point)
{
register ULONG result;

	if(!w3dpatch)	return( W3D_DrawPoint(context,point) );

	if(point->pointsize>=3.0)
		result=DrawDisc(context,&point->v1,point->tex,point->pointsize);
	else
		result=DrawLine(context,&point->v1,&point->v1,point->tex,point->pointsize);

	return(result);
}
/*==========================================================================*/
ULONG PatchW3D_ReadZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *z)
{
register UBYTE *zbuffer;
register ULONG i;
register float  *fz;
register double *dz;
register  WORD  *wz;
register UWORD  *uwz;
register ULONG  *ulz;
UBYTE t[8];
ULONG zsize;
ULONG result;

	if(!w3dpatch)	return( W3D_ReadZSpan(context,x,y,n,z) );

	if(context->zbuffer==NULL) return(W3D_NOZBUFFER);

	if(zformat== 0) findzformat(context);
	if(zformat==-1) return(W3D_NOZBUFFER);
	result=W3D_SUCCESS;
	zsize=zsizes[zformat]/8;

	zbuffer=context->zbuffer+((y*context->width+x)*zsize);	/* offset from start */

	if(zformat==ZFLOAT)
	ILOOP(n)
		{
		fz=(float*)zbuffer;
		z[i]=(double)fz[0]; zbuffer+=zsize;
		}

	if(zformat==ZFLOATPC)
	ILOOP(n)
		{
		fz=(float*)t;
		t[3]=zbuffer[0]; t[2]=zbuffer[1]; t[1]=zbuffer[2]; t[0]=zbuffer[3];
		z[i]=(double)fz[0]; zbuffer+=zsize;
		}

	if(zformat==ZDOUBLE)
	ILOOP(n)
		{
		dz=(double*)zbuffer;
		z[i]=(double)dz[0]; zbuffer+=zsize;
		}

	if(zformat==ZDOUBLEPC)
	ILOOP(n)
		{
		dz=(double*)t;
		t[7]=zbuffer[0]; t[6]=zbuffer[1]; t[5]=zbuffer[2]; t[4]=zbuffer[3]; t[3]=zbuffer[4]; t[2]=zbuffer[5]; t[1]=zbuffer[6]; t[0]=zbuffer[7];
		z[i]=dz[0]; zbuffer+=zsize;
		}

	if(zformat==ZWORD)
	ILOOP(n)
		{
		wz=(WORD*)zbuffer;
		z[i]=((double)wz[0])/32767.0; zbuffer+=zsize;
		}

	if(zformat==ZWORDPC)
	ILOOP(n)
		{
		wz=(WORD*)t;
		t[1]=zbuffer[0]; t[0]=zbuffer[1];
		z[i]=((double)wz[0])/32767.0; zbuffer+=zsize;
		}

	if(zformat==ZUWORD)
	ILOOP(n)
		{
		uwz=(UWORD*)zbuffer;
		z[i]=((double)uwz[0])/65535.0;  zbuffer+=zsize;
		}

	if(zformat==ZUWORDPC)
	ILOOP(n)
		{
		uwz=(UWORD*)t;
		t[1]=zbuffer[0]; t[0]=zbuffer[1];
		z[i]=((double)uwz[0])/65535.0; zbuffer+=zsize;
		}

	if(zformat==Z24S8)
	ILOOP(n)
		{
		ulz=(ULONG*)t;
		t[0]=0; t[1]=zbuffer[0]; t[2]=zbuffer[1]; t[3]=zbuffer[2];
		z[i]=((double)ulz[0])/16777215.0; zbuffer+=zsize;
		}

	if(zformat==Z24S8PC)
	ILOOP(n)
		{
		ulz=(ULONG*)t;
		t[0]=0; t[1]=zbuffer[3]; t[2]=zbuffer[2]; t[3]=zbuffer[1];
		z[i]=((double)ulz[0])/16777215.0; zbuffer+=zsize;
		}

	if(zformat==S8Z24)
	ILOOP(n)
		{
		ulz=(ULONG*)t;
		t[0]=0; t[1]=zbuffer[1]; t[2]=zbuffer[2]; t[3]=zbuffer[3];
		z[i]=((double)ulz[0])/16777215.0; zbuffer+=zsize;
		}

	if(zformat==S8Z24PC)
	ILOOP(n)
		{
		ulz=(ULONG*)t;
		t[0]=0; t[1]=zbuffer[0]; t[2]=zbuffer[1]; t[3]=zbuffer[2];
		z[i]=((double)ulz[0])/16777215.0; zbuffer+=zsize;
		}

	return(result);
}
/*==========================================================================*/
void PatchW3D_WriteZSpan(W3D_Context *context, ULONG x, ULONG y,ULONG n, W3D_Double *z, UBYTE *mask)
{
register UBYTE *zbuffer;
register UBYTE *m=mask;
register ULONG i;
UBYTE t[8];
register float  *fz;
register double *dz;
register  WORD  *wz;
register UWORD  *uwz;
register ULONG  *ulz;
UBYTE *z8;
ULONG zsize;

	if(!w3dpatch)	{W3D_WriteZSpan(context,x,y,n,z,mask); return;}

	if(context->zbuffer==NULL) return;

	if(zformat==0) findzformat(context);
	if(zformat==-1)
	{
	z8=context->zbuffer;
	printf("Unknown zformat! Zbuffer contain [%d %d %d %d] [%d %d %d %d]...\n",z8[0],z8[1],z8[2],z8[3],z8[4],z8[5],z8[6],z8[7]);
	return;
	}
	zsize=zsizes[zformat]/8;

	zbuffer=context->zbuffer+((y*context->width+x)*zsize);	/* offset from start */

	if(zformat==ZFLOAT)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			fz=(float*)zbuffer;
			fz[0]=(float)z[i];
			}
		zbuffer+=zsize;
		}

	if(zformat==ZFLOATPC)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			fz=(float*)t;
			fz[0]=(float)z[i];
			zbuffer[3]=t[0]; zbuffer[2]=t[1]; zbuffer[1]=t[2]; zbuffer[0]=t[3];
	 		}
		zbuffer+=zsize;
		}

	if(zformat==ZDOUBLE)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			dz=(double*)zbuffer;
			dz[0]=(double)z[i];
			}
		zbuffer+=zsize;
		}

	if(zformat==ZDOUBLEPC)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			dz=(double*)t;
			dz[0]=(double)z[i];
			zbuffer[7]=t[0]; zbuffer[6]=t[1]; zbuffer[5]=t[2]; zbuffer[4]=t[3];zbuffer[3]=t[4]; zbuffer[2]=t[5]; zbuffer[1]=t[6]; zbuffer[0]=t[7];
	 		}
		zbuffer+=zsize;
		}

	if(zformat==ZWORD)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			wz=(WORD*)zbuffer;
			wz[0]=(WORD)(z[i]*32767.0);
			}
		zbuffer+=zsize;
		}

	if(zformat==ZWORDPC)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			wz=(WORD*)t;
			wz[0]=(WORD)(z[i]*32767.0);
			zbuffer[1]=t[0]; zbuffer[0]=t[1];
	 		}
		zbuffer+=zsize;
		}

	if(zformat==ZUWORD)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			uwz=(UWORD*)zbuffer;
			uwz[0]=(UWORD)(z[i]*65535.0);
			}
		zbuffer+=zsize;
		}

	if(zformat==ZUWORDPC)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			uwz=(UWORD*)t;
			uwz[0]=(UWORD)(z[i]*65535.0);
			zbuffer[1]=t[0]; zbuffer[0]=t[1];
	 		}
		zbuffer+=zsize;
		}

	if(zformat==Z24S8)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			ulz=(ULONG*)t;
			ulz[0]=(ULONG)(z[i]*16777215.0);
			zbuffer[0]=t[1]; zbuffer[1]=t[2]; zbuffer[2]=t[3];
	 		}
		zbuffer+=zsize;
		}

	if(zformat==Z24S8PC)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			ulz=(ULONG*)t;
			ulz[0]=(ULONG)(z[i]*16777215.0);
			zbuffer[3]=t[1]; zbuffer[2]=t[2]; zbuffer[1]=t[3];
	 		}
		zbuffer+=zsize;
		}

	if(zformat==S8Z24)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			ulz=(ULONG*)t;
			ulz[0]=(ULONG)(z[i]*16777215.0);
			zbuffer[1]=t[1]; zbuffer[2]=t[2]; zbuffer[3]=t[3];
		 	}
		zbuffer+=zsize;
		}

	if(zformat==S8Z24PC)
	ILOOP(n)
		{
		if(m[i]!=0)
			{
			ulz=(ULONG*)t;
			ulz[0]=(ULONG)(z[i]*16777215.0);
			zbuffer[0]=t[1]; zbuffer[1]=t[2]; zbuffer[2]=t[3];
	 		}
		zbuffer+=zsize;
		}

}
/*==========================================================================*/
ULONG PatchW3D_ReadZPixel(W3D_Context *context, ULONG x, ULONG y,W3D_Double *dz)
{
	if(!w3dpatch)	return( W3D_ReadZPixel(context,x,y,dz) );

	return(PatchW3D_ReadZSpan(context,x,y,1,dz));
}
/*==========================================================================*/
void PatchW3D_WriteZPixel(W3D_Context *context, ULONG x, ULONG y,W3D_Double *dz)
{
UBYTE mask=1;

	if(!w3dpatch)	{W3D_WriteZPixel(context,x,y,dz); return;}

	PatchW3D_WriteZSpan(context,x,y,1,dz,&mask);
}
/*==========================================================================*/
void findzformat(W3D_Context *context)
{
#define TESTZ 0.1
W3D_Double dz=TESTZ;
ULONG i;

	zformat=-1;
	if(W3D_ClearZBuffer(context,&dz)!=W3D_SUCCESS)	/* put a given value in zbuffer */
		{printf("no zbuffer so cant find z format\n");return;}
	ILOOP(12)
		{
		zformat=i+1; 						/* try all z formats */
		PatchW3D_ReadZPixel(context,0,0,&dz);		/* try to re-read the zbuffer with this format */
		if((TESTZ-0.001) < dz )					/* is it aproximately the same value  ? */
		if(dz < (TESTZ+0.001) )
			{printf("found z format %ld\n",zformat); return;}
		}
	zformat=-1;
	printf("cant find z format\n");
}
/*==========================================================================*/

