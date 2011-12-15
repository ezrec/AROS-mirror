#include <intuition/intuition.h>
#include "rubik.h"

extern struct RastPort *rp[2];
extern UWORD frametoggle, framecount, newcube;
extern struct GfxBase * GfxBase;
	
void slice00init(void);
void slice10init(void);
void slice20init(void);
void slice01init(void);
void slice11init(void);
void slice21init(void);
void slice02init(void);
void slice12init(void);
void slice22init(void);
void dopoints(struct Matrix *bm, struct Vector *bp, WORD numpoints, struct Vector *pointstart[], struct Vector *bufpoints);
void matrix(struct Vector *dest,struct Vector *src, struct Matrix *bm);


void doobject(rap, objectinfo, dorot)
struct RastPort *rap;
struct Objectinfo *objectinfo; 
int dorot; {
	struct Vector **nextp;
	WORD tcount = 0, *nextcolor;

	/* The first time through, newcube = TRUE, and the rotated points
	   are computed and placed in the buffer.  Thereafter, newcube =
	   FALSE, and the existing values are used for extra speed. */
	if (newcube) {
		/* First update the display matrix for this slice */
		if (dorot) {	/* Call the object's procedure */
				void (*function)();

				function = objectinfo->procedure;
				(*function)(objectinfo->matrix, SINA, COSA);
		}
		/* Now apply this matrix to all the vertices in the slice,
		   putting the results in bufpoints. */
		dopoints(objectinfo->matrix, objectinfo->position,
			objectinfo->numpoints, objectinfo->points,
			objectinfo->bufpoints);
	}
	/* Draw the slice by traversing the tile list */
 	/* Initialize buffer pointers */
	nextcolor = objectinfo->colorbuf;
	nextp = objectinfo->pptrbuf;
	for (tcount=0; tcount<objectinfo->numtiles; tcount++) {
		struct Vector *c0, *c1, *c2;
		WORD vc;
		long tcolor;

		/* Backface removal */
		/* Construct normal to Tile using vector cross product, and
		   test its z component.  If normal faces away from the
		   camera, skip the tile. */
		c0 = nextp[0] + framecount; c1 = nextp[1] + framecount;
		c2 = nextp[2] + framecount;
		if (((c1->x - c0->x) * (c2->y - c1->y) - 
		     (c2->x - c1->x) * (c1->y - c0->y)) >= 0) {
			nextcolor++;
			nextp += 4;	/* Skip to start of next Tile */
			continue;
		}
		tcolor = *nextcolor;
		for (vc=0; vc<4; vc++) {
			long x, y;

			/* Draw tile */
			SetAPen(rap, tcolor);
			/* Place origin at center of raster */
			x = (*nextp + framecount)->x + 128L;
			y = (*nextp + framecount)->y + 100L;
			if (vc==0) AreaMove(rap, x, y); /* Start tile */
			else AreaDraw(rap, x, y);	/* Continue tile */
			nextp++;
		}
	 	AreaEnd(rap);	/* End tile */
		nextcolor++;
	}
}

WORD mul3d(a,b) WORD a,b; {	/* Fixed point multiplication */
	LONG c;
	c = a;
	c *= b;
	c += 0x2000;		/* Add .5 ulp for round off */
	c >>= 14;
	return((WORD)c);
}

void xyrot(bm, sine, cosine) struct Matrix *bm; WORD sine, cosine; {
	WORD tmp;
	tmp = mul3d(bm->uv11,cosine) - mul3d(bm->uv12,sine);
	bm->uv12 = mul3d(bm->uv11,sine) + mul3d(bm->uv12,cosine);
	bm->uv11 = tmp;
	tmp = mul3d(bm->uv21,cosine) - mul3d(bm->uv22,sine);
	bm->uv22 = mul3d(bm->uv21,sine) + mul3d(bm->uv22,cosine);
	bm->uv21 = tmp;
	tmp = mul3d(bm->uv31,cosine) - mul3d(bm->uv32,sine);
	bm->uv32 = mul3d(bm->uv31,sine) + mul3d(bm->uv32,cosine);
	bm->uv31 = tmp;
}

void zxrot(bm, sine, cosine) struct Matrix *bm; WORD sine, cosine; {
	WORD tmp;
	tmp = mul3d(bm->uv13,cosine) - mul3d(bm->uv11,sine);
	bm->uv11 = mul3d(bm->uv13,sine) + mul3d(bm->uv11,cosine);
	bm->uv13 = tmp;
	tmp = mul3d(bm->uv23,cosine) - mul3d(bm->uv21,sine);
	bm->uv21 = mul3d(bm->uv23,sine) + mul3d(bm->uv21,cosine);
	bm->uv23 = tmp;
	tmp = mul3d(bm->uv33,cosine) - mul3d(bm->uv31,sine);
	bm->uv31 = mul3d(bm->uv33,sine) + mul3d(bm->uv31,cosine);
	bm->uv33 = tmp;
}

void yzrot(bm, sine, cosine) struct Matrix *bm; WORD sine, cosine; {
	WORD tmp;
	tmp = mul3d(bm->uv12,cosine) - mul3d(bm->uv13,sine);
	bm->uv13 = mul3d(bm->uv12,sine) + mul3d(bm->uv13,cosine);
	bm->uv12 = tmp;
	tmp = mul3d(bm->uv22,cosine) - mul3d(bm->uv23,sine);
	bm->uv23 = mul3d(bm->uv22,sine) + mul3d(bm->uv23,cosine);
	bm->uv22 = tmp;
	tmp = mul3d(bm->uv32,cosine) - mul3d(bm->uv33,sine);
	bm->uv33 = mul3d(bm->uv32,sine) + mul3d(bm->uv33,cosine);
	bm->uv32 = tmp;
}

void perspect(dest) struct Vector *dest; {
	LONG zinv = 0x00400000;		/* 1.0 */
	dest->z = (dest->z < 64) ? 64 : dest->z;
	zinv /= 0x200 + dest->z;
	dest->x = mul3d(dest->x,(WORD)zinv);
	dest->y = mul3d(dest->y,(WORD)zinv);
}

void addvect(bp, src, dest) struct Vector *bp, *src, *dest; {
	dest->x = src->x + bp->x;
	dest->y = src->y + bp->y;
	dest->z = src->z + bp->z;
}

void matmult(dest, src1, src2) struct Matrix *dest, *src1, *src2; {
	matrix((struct Vector *)&dest->uv11, (struct Vector *)&src1->uv11, src2);
	matrix((struct Vector *)&dest->uv21, (struct Vector *)&src1->uv21, src2);
	matrix((struct Vector *)&dest->uv31, (struct Vector *)&src1->uv31, src2);
}

void matrix(dest, src, bm)
struct Vector *dest, *src;
struct Matrix *bm;
{
	dest->x = mul3d(src->x,bm->uv11) + mul3d(src->y,bm->uv12) +
			mul3d(src->z,bm->uv13);
	dest->y = mul3d(src->x,bm->uv21) + mul3d(src->y,bm->uv22) +
			mul3d(src->z,bm->uv23);
	dest->z = mul3d(src->x,bm->uv31) + mul3d(src->y,bm->uv32) +
			mul3d(src->z,bm->uv33);
}

void dopoints(bm, bp, numpoints, pointstart, bufpoints)
struct Matrix *bm;
struct Vector *bp, *pointstart[], *bufpoints;
WORD numpoints; 
{
	WORD pcount, poff;
	/* Rotate, translate, and perspect each point */
	for (pcount=0; pcount < numpoints; pcount++) {
		poff = framecount + pcount * MAXFRAMES;
		matrix(&bufpoints[poff],pointstart[pcount], bm);
		addvect(bp, &bufpoints[poff], &bufpoints[poff]);
		perspect(&bufpoints[poff]);
	}
}

struct Vector cubeposition = {0,0,0x200}, cameraposition = {0,0,0};
struct Matrix cameramatrix = 
	{0x2d41, 0x0000, 0x2d41,	/* 45 deg rot in x and z  */
	 0x16a0, 0x376d,-0x16a0,	/* followed by 30 deg rot */
	-0x2731, 0x2000, 0x2731};	/* in y and z */

struct Vector p[4][4][4], *vf[3][3][3][4], *vb[3][3][3][4];
struct Tile ff[3][3][3], fb[3][3][3];
struct Matrix slicematrix[3][3];
struct Vector *slicepoints[3][3][32];
struct Tile *slicetiles[3][3][21];
struct Objectinfo SliceInfo[3][3];

void matrixinit(um) struct Matrix *um; {*um = cameramatrix;}

void cubeinit() {
	WORD i, j, k, l, a1, s1, count=0;
	for (i=0; i<4; i++) for (j=0; j<4; j++) {
		for (k=0; k<4; k++) {
			p[i][j][k].x = 0x38 * (-3 + 2*i);
			p[i][j][k].y = 0x38 * (-3 + 2*j);
			p[i][j][k].z = 0x38 * (-3 + 2*k);
		}
		for (k=0; k<2; k++) {
			slicepoints[2][0][count+k] = &p[i][j][k];
			slicepoints[1][0][count+k] = &p[i][j][k+1];
			slicepoints[0][0][count+k] = &p[i][j][k+2];
			slicepoints[0][1][count+k] = &p[k][i][j];
			slicepoints[1][1][count+k] = &p[k+1][i][j];
			slicepoints[2][1][count+k] = &p[k+2][i][j];
			slicepoints[2][2][count+k] = &p[j][k][i];
			slicepoints[1][2][count+k] = &p[j][k+1][i];
			slicepoints[0][2][count+k] = &p[j][k+2][i];
		}
		count += 2;
	}
	count = 0;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			SliceInfo[i][j].matrix = &slicematrix[i][j];
			matrixinit(&slicematrix[i][j]);
			SliceInfo[i][j].position = &cubeposition;
			SliceInfo[i][j].numpoints = 32;
			SliceInfo[i][j].points = slicepoints[i][j];
			SliceInfo[i][j].tiles = slicetiles[i][j];
			for (k=0; k<4; k++) {
				l = (k>1) ? 3 - k : k; 	/* 0 1 1 0 */
				vf[0][i][j][k] = &p[3][i+l][j+k/2];
				vf[1][i][j][k] = &p[j+k/2][3][i+l];
				vf[2][i][j][k] = &p[i+l][j+k/2][3];
				vb[0][i][j][k] = &p[0][j+k/2][i+l];
				vb[1][i][j][k] = &p[i+l][0][j+k/2];
				vb[2][i][j][k] = &p[j+k/2][i+l][0];
			}
			for (k=0; k<3; k++) {
				ff[i][j][k].vertexstart = vf[i][j][k];
				ff[i][j][k].color = i + 2;
				fb[i][j][k].vertexstart = vb[i][j][k];
				fb[i][j][k].color = i + 5;
				a1 = (j + 1)%3;
				s1 = (j==1) ? k : 2-k;
				slicetiles[k][j][count]   = &ff[j][i][s1];
				slicetiles[k][j][count+1] = &ff[a1][s1][i];
				slicetiles[k][j][count+2] = &fb[j][s1][i];
				slicetiles[k][j][count+3] = &fb[a1][i][s1];
			}
		}
		count += 4;
		SliceInfo[i][0].procedure = (void *)&xyrot;
		SliceInfo[i][1].procedure = (void *)&yzrot;
		SliceInfo[i][2].procedure = (void *)&zxrot;
	}
	slice00init(); slice10init(); slice20init();
	slice01init(); slice11init(); slice21init();
	slice02init(); slice12init(); slice22init();
}

struct Vector
	*vt10[4] = {&p[0][0][1], &p[0][3][1], &p[3][3][1], &p[3][0][1]},
	*vt00[4] = {&p[0][0][2], &p[0][3][2], &p[3][3][2], &p[3][0][2]},
	*vt01[4] = {&p[1][3][0], &p[1][3][3], &p[1][0][3], &p[1][0][0]},
	*vt11[4] = {&p[2][3][0], &p[2][3][3], &p[2][0][3], &p[2][0][0]},
	*vt12[4] = {&p[0][1][0], &p[3][1][0], &p[3][1][3], &p[0][1][3]},
	*vt02[4] = {&p[0][2][0], &p[3][2][0], &p[3][2][3], &p[0][2][3]};

struct Tile
	ft00 = {vt00,1}, ft10 = {vt10,1}, ft01 = {vt01,1},
	ft11 = {vt11,1}, ft02 = {vt02,1}, ft12 = {vt12,1};

void slice20init() {
	WORD i,j, count=12;

	for (i=0; i<3; i++) for (j=0; j<3; j++) 
		slicetiles[2][0][count++] = &fb[2][i][j];
	SliceInfo[2][0].numtiles = count;
}

void slice10init() {
	WORD count=12;

	slicetiles[1][0][count++] = &ft10;
	SliceInfo[1][0].numtiles = count;
}

void slice00init() {
	WORD count=12;

	slicetiles[0][0][count++] = &ft00;
	SliceInfo[0][0].numtiles = count;
}

void slice01init() {
	WORD count=12;

	slicetiles[0][1][count++] = &ft01;
	SliceInfo[0][1].numtiles = count;
}

void slice11init() {
	WORD count=12;

	slicetiles[1][1][count++] = &ft11;
	SliceInfo[1][1].numtiles = count;
}

void slice21init() {
	WORD i,j, count=12;

	for (i=0; i<3; i++) for (j=0; j<3; j++) 
		slicetiles[2][1][count++] = &ff[0][i][j];
	SliceInfo[2][1].numtiles = count;
}

void slice22init() {
	WORD i,j, count=12;

	for (i=0; i<3; i++) for (j=0; j<3; j++)
		slicetiles[2][2][count++] = &fb[1][i][j];
	SliceInfo[2][2].numtiles = count;
}

void slice12init() {
	WORD count=12;

	slicetiles[1][2][count++] = &ft12;
	SliceInfo[1][2].numtiles = count;
}

void slice02init() {
	WORD count=12;

	slicetiles[0][2][count++] = &ft02;
	SliceInfo[0][2].numtiles = count;
}
