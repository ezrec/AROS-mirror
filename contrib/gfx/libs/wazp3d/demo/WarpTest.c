/*
	WarpTest
	This program is part of the Warp3D project.
	Copyright © 1998 Thomas Frieden, Hans-Jörg Frieden, Sam Jordan
	You may freely modify and/or use any part of this file. It
	is considered an example for programming Warp3D.

	This is a small program that shows some effects that can be done
	with Warp3D, and may also serve as an example for programming
	with Warp3D. It demonstrates fog, alpha blending, texture mapping,
	both linear and perspective corrected, gouraud shading, filtering,
	light mapping, mipmapping, including automatic mipmap generation,
	and double buffering. Here`s how to use it:
	From shell, enter WarpTest <texture.ppm>. If the texture is omitted,
	wall.ppm is taken as default.
	The wall can be moved and rotated with the cursor keys. Pressing the
	mouse button and moving the mouse also rotates the wall. You can also
	use the following keys for switching different effects:

		f - Toggle fogging.
		8 - Push back the fog end value (observe the second last text row)
		5 - Push fog end towards viewer
		9 - Push back fog start
		6 - Push fog start towards viewer

		t - toggle texture mapping/gouraud shading
		e - toggle light mapping
		p - toggle linear/perspective corrected mapping

		l - turn on alpha blending
		1 - set filtering to W3D_NEAREST (no texture filtering)
		2 - set filtering to W3D_LINEAR (bilinear texture
		3 - set filtering to W3D_NEAREST_MIP_NEAREST (mipmapping, no filtering)
		4 - set filtering to W3D_LINEAR_MIP_LINEAR (trilinear filtering with
												mipmapping)

		a - show mipmaps (don`t mind the flickering, they are only drawn
												to the first buffer)
		b - show vertex coordinates (not very useful)
		c - slow motion
		d - draw the background pattern

		Pressing 'a' shows the current texture, along with the mipmaps
		that were automatically created. These mipmaps are created with
		a simple median where four pixels are summed up and divided by
		four to produce a new pixel.

	History
	-------
	6-Aug-98        First public developer release

	8-Aug-98        Added a background patter switch (red lines), plus a
					key ('d') to toggle it. Additionally, Opacity for
					textures was replaced with a gradient pattern running
					from top-left to bottom-right.

	13-Sep-98       ZBuffering now works
	28-Sep-98       Pressing F not switches hinting about fogging (press twice,
					I wa too lazy to correct the bug :)
	 3-Oct-98       Added TEXTURE and ZBUFFER command line switches
	12-Jan-99       Added Outline mode with W3D_DrawLineLoop
					Added V2 driver interface
					Increased library minimum version to 2.0
					Added information printing about drivers
					Added example for W3D_QueryDriver
	20-Feb-99       ClearZBuffer is now executed inside a hardware lock,
					so this shouldn't cause anymore problems (fingers crossed)
	28-Jan-09	Port to AROS, double buffering implemented with BlitBitMapRastPort().
			Warnings and unused code removed.

*/
;// Includes
#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/asl.h>
#include <proto/arossupport.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/Warp3D.h>
#include <cybergraphx/cybergraphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

;;//
;// Stuff

char version[] = "$VER: WarpTest 3.0 (28.01.2009)";

struct Library *Warp3DBase;
struct Library *CyberGfxBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct DosLibrary *DOSBase;
struct Screen *screen;
struct Window *window;
BOOL outline = FALSE;
BOOL zb = FALSE;

struct RDArgs *rda = NULL;
#define TEMPLATE "TEXTURE,ZBUFFER/S,REPEAT/N,LIGHTMAP/K"

ULONG texturesize;
UBYTE *texture;
UBYTE *lightmap;
IPTR result[10];

W3D_Texture *tex, *lighttex = NULL;
W3D_Triangle tri;
//W3D_Point point;
W3D_Color ccol = {0.0, 1.0, 0.0, 0.8};
W3D_Double zdepth = 0.8;
ULONG zmode = 0;

ULONG DLevel=0, DWrap=0;
int persp = 0;
int forcelin = 0;
float divisor = 2048.0;
float factor = 16.f;
float tmax   = 256.f;
float lastw  = 0.f;
float lastd  = 0.f;
ULONG CurrentBlend = 0;
ULONG BlendModes[] = {W3D_REPLACE, W3D_DECAL, W3D_MODULATE, W3D_BLEND};
ULONG CurrentFog = 0;
ULONG FogModes[] = {0, W3D_FOG_LINEAR, W3D_FOG_EXP, W3D_FOG_EXP_2, W3D_FOG_INTERPOLATED};
W3D_Double LastZ = 0.0;
int curfilt = 1;
int drag = 0;
int push = 0;
int second_wall = 0;
int pixels = 1;

const float Back  = 40.0;
const float Front = 0.5;

#define kzen (-65535.0*(Back*Front)/(Back-Front))
#define z0   ( 65535.0*Back/(Back-Front))
#define SCALE 240

typedef struct {
	float x,y,z;
	float u,v;
	float iz;
} Vector3;

typedef struct {
	float m11, m12, m13;
	float m21, m22, m23;
	float m31, m32, m33;
} Matrix3;

Vector3 Square[4] = {
	// x     y    z      u      v
	{-1.f,  1.f, 0.f,   0.f,   0.f},
	{ 1.f,  1.f, 0.f,   1.f,   0.f},
	{ 1.f, -1.f, 0.f,   1.f,   1.f},
	{-1.f, -1.f, 0.f,   0.f,   1.f}
};

Vector3 LSquare[4] = {
	// x     y    z      u      v
	{-1.f,  1.f, 0.f,   0.f,   0.f},
	{ 1.f,  1.f, 0.f,   1.f,   0.f},
	{ 1.f, -1.f, 0.f,   1.f,   1.f},
	{-1.f, -1.f, 0.f,   0.f,   1.f}
};


Vector3 TempSquare[4];

Vector3 Pos = {0.f, 0.f, 3.f, 0.f, 0.f};    // Position of wall
float theta = 0.0;                          // Wall Angle

int aflag=0, bflag=0, cflag=0, dflag=0;

ULONG FogQuality = W3D_H_NICE;
W3D_Fog myFog = {
	1.0,0.2,1.0,
	{8.1/32.0, 8.1/32.0, 17.1/32.0}
};


void *texmap;

;;//
;// Matrix/Vector stuff
void MatVec(Matrix3 *M, Vector3 *v, Vector3 *r)
{
	// Calcuate r=Mv
	r->x = M->m11*v->x + M->m12*v->y + M->m13*v->z;
	r->y = M->m21*v->x + M->m22*v->y + M->m23*v->z;
	r->z = M->m31*v->x + M->m32*v->y + M->m33*v->z;
}

void RotYMat(Matrix3 *M, float theta)
// Create a Y axis rotation matrix by angle theta (theta in degrees)
{

	theta = theta/180.f * M_PI;

	M->m11 =  (float)cos(theta);
	M->m13 = -(float)sin(theta);
	M->m31 =  (float)sin(theta);
	M->m33 =  (float)cos(theta);
	M->m22 =  1.f;

	M->m12 =
	M->m21 =
	M->m23 =
	M->m32 = 0.f;
}

void Project(Vector3 *v)
{
	if (v->z == 0.f) v->z+=0.0001;
	v->iz = 1.0/v->z;
	v->x = 320+SCALE*v->x*(v->iz);
	v->y = 240-SCALE*v->y*(v->iz);
	v->z = ((kzen/v->z)+z0)/65536.f;
}

void VecAdd(Vector3 *a, Vector3 *b, Vector3 *r)
// Calcuate r=a+b;
// I love C++!!!
{
	r->x = a->x + b->x;
	r->y = a->y + b->y;
	r->z = a->z + b->z;
}

void Transform(Vector3 Pos, float theta)
// Transform the wall to the desired orientation
{
	Matrix3 M;
	int i;

	RotYMat(&M, theta);     // Create rotation matrix;
	for (i=0; i<4; i++) {
		MatVec(&M, &Square[i], &TempSquare[i]);
		VecAdd(&TempSquare[i], &Pos, &TempSquare[i]);
	//}
	//for (i=0; i<4; i++) {
		Project(&TempSquare[i]);
	}
}

;;//
;// "LoadTextureFromPPM"
#define ARGB8888toARGB1555(ic) (0x8000 | ((ic&0xF8)>>3) | ((ic&0xF800)>>6) | ((ic&0xF80000)>>9))
#define ARGB8888toARGB4444(ic) ((ic&0xF0000000)>>16 | \
								(ic&0x00F00000)>>12 | \
								(ic&0x0000F000)>>8  | \
								(ic&0x000000F0)>>4)
UBYTE *LoadTextureFromPPM(APTR where, char *filename, UBYTE Opacity, int *w, int *h)
{
	ULONG *map, *map2;
	FILE *f;
	int i,j;
	UBYTE r,g,b;
	unsigned long x,y;
	int opqy, opqx, o;

	f=fopen(filename, "r");

	if (!f) {
		printf("Error: Unable to open file '%s'\n", filename);
		return NULL;
	}

#ifndef __STORM__
	i=fscanf(f, "P6\n%ld %ld\n255\n", &x, &y);
#else
	i=fscanf(f, "P6\n%ld\n%ld\n255\n", &x, &y);
#endif

	if (i!=2) {
		printf("Error: This seems to be no PPM file\n");
		fclose(f);
		return NULL;
	}

	*w = x;
	*h = y;

	if (where==NULL) {
		map2 = map = AllocVec(x*y*4, MEMF_PUBLIC);
	} else {
		map = map2 = where;
	}
	if (!map) {
		fclose(f);
		printf("Error: Out of memory\n");
		return NULL;
	}

	opqy = 256/y;
	opqx = 256/x;
	opqx = opqx>>1;
	o=0;
	for (i=0; i<y; i++) {
		for(j=0; j<x; j++) {
			if (o+j*opqy > 255)
				Opacity = 255;
			else
				Opacity = (UBYTE)(o+j*opqy);

			r=(UBYTE)fgetc(f);
			g=(UBYTE)fgetc(f);
			b=(UBYTE)fgetc(f);
#if AROS_BIG_ENDIAN			
			*map = (Opacity<<24) | ((r&0xFF)<<16) | ((g&0xFF)<<8) | (b&0xFF);
#else
			*map = (Opacity&0xFF) | ((r&0xFF)<<8) | ((g&0xFF)<<16) | ((b&0xFF)<<24);
#endif
//            *map = (Opacity<<8) | ((r&0xFF)) | ((g&0xFF)<<24) | (b&0xFF)<<16;
			map++;
		}
		o += opqx;
	}

	fclose(f);
	return (UBYTE *)map2;
}

BOOL GenTexture(W3D_Context* context, char* name, LONG repeat)
{
	UBYTE *where;
	int w, h;
	ULONG error;
	int i;

	// This loads a ppm (24 bit) texture file from disk, and sets
	// it to 50% transparency (0x7F)...
	where = LoadTextureFromPPM(NULL, name, 0x7F, &w, &h);
	if (!where) return FALSE;
	printf("Size: %d×%d\n", w,h);

	// This sets the square's texture coordinates to the right pixel values
	for (i=0;i<4; i++) {
		Square[i].u *= (w-1);      //  -1
		Square[i].v *= (h-1);      //  -1
		if (repeat > 1) {
			Square[i].u *= repeat;
			Square[i].v *= repeat;
		}
	}


	texmap = where;

	tex = W3D_AllocTexObjTags(context, &error,
		W3D_ATO_IMAGE,      (IPTR)where,    // The image data
		W3D_ATO_FORMAT,     W3D_A8R8G8B8,   // This is a 32 bit image
		W3D_ATO_WIDTH,      w,              // Length of one side
		W3D_ATO_HEIGHT,     h,
		W3D_ATO_MIPMAP,     0xffff,         // Mipmap mask - see autodocs
	TAG_DONE);

	printf("Texture created\n");

	if (!tex || error != W3D_SUCCESS) {
		printf("Error generating texture: ");
		switch(error) {
		case W3D_ILLEGALINPUT:
			printf("Illegal input\n");
			break;
		case W3D_NOMEMORY:
			printf("Out of memory\n");
			break;
		case W3D_UNSUPPORTEDTEXSIZE:
			printf("Unsupported texture size\n");
			break;
		case W3D_NOPALETTE:
			printf("Chunky texture without palette specified\n");
			break;
		case W3D_UNSUPPORTEDTEXFMT:
			printf("Texture format not supported\n");
			break;
		default:
			printf("ahem... An error has occured\n");
		}
		return FALSE;
	}

	W3D_UploadTexture(context, tex);

	return TRUE;
}

;;//
;// GenLightmap
// Ok, this is a cheap, but who cares :)
// In a real application, you would have this code only once
BOOL GenLightmap(W3D_Context* context, char* name, LONG repeat)
{
#define LEVEL a
	UBYTE *where, *here;
	int w, h;
	ULONG error;
	int i;
	int a;

	// This loads a ppm (24 bit) texture file from disk, and sets
	// it to 50% transparency (0x7F)...
	where = LoadTextureFromPPM(NULL, name, 0x7F, &w, &h);
	if (!where) return FALSE;
	printf("Size: %d×%d\n", w,h);

	// Convert it to pure alpha
	// Should really use a W3D_A8 here, but I'm too lazy
	here = where;
	for (i = 0; i < w*h; i++) {
		a = (here[1] + here[2] + here[3])/3;
		here[0] = a;
		here[1] = LEVEL;
		here[2] = LEVEL;
		here[3] = LEVEL;
		here += 4;
	}


	// This sets the square's texture coordinates to the right pixel values
	for (i=0;i<4; i++) {
		LSquare[i].u *= (w-1);      //  -1
		LSquare[i].v *= (h-1);      //  -1
		if (repeat > 1) {
			LSquare[i].u *= repeat;
			LSquare[i].v *= repeat;
		}
	}


	lightmap = where;

	lighttex = W3D_AllocTexObjTags(context, &error,
		W3D_ATO_IMAGE,      (IPTR)where,    // The image data
		W3D_ATO_FORMAT,     W3D_A8R8G8B8,   // This is a 32 bit image
		W3D_ATO_WIDTH,      w,              // Length of one side
		W3D_ATO_HEIGHT,     h,
		W3D_ATO_MIPMAP,     0xffff,         // Mipmap mask - see autodocs
	TAG_DONE);

	if (!lighttex || error != W3D_SUCCESS) {
		printf("Error generating light texture: ");
		switch(error) {
		case W3D_ILLEGALINPUT:
			printf("Illegal input\n");
			break;
		case W3D_NOMEMORY:
			printf("Out of memory\n");
			break;
		case W3D_UNSUPPORTEDTEXSIZE:
			printf("Unsupported texture size\n");
			break;
		case W3D_NOPALETTE:
			printf("Chunky texture without palette specified\n");
			break;
		case W3D_UNSUPPORTEDTEXFMT:
			printf("Texture format not supported\n");
			break;
		default:
			printf("ahem... An error has occured\n");
		}
		return FALSE;
	}
	return TRUE;
}

;;//
;// PrintInfo
void PrintInfo(W3D_Context* context, struct RastPort *rp)
{
	int x,y;
	static char buffer[256];

	SetAPen(rp, 2);
	SetDrMd(rp, JAM1);
	x=450; y=100;

	if (W3D_GetState(context, W3D_FAST) == W3D_ENABLED)
					   sprintf(buffer, "Fast mode");
	else               sprintf(buffer, " ");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	if (outline)       sprintf(buffer, "Outline mode");
	else               sprintf(buffer, " ");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	if (aflag)         sprintf(buffer, "Show MipMaps");
	else               sprintf(buffer, " ");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	if (bflag)         sprintf(buffer, "Vertex coordinates");
	else               sprintf(buffer, " ");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	if (cflag)         sprintf(buffer, "Slow motion");
	else               sprintf(buffer, " ");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	if (W3D_GetState(context, W3D_PERSPECTIVE) == W3D_ENABLED)
		sprintf(buffer, "Perspective");
	else
		sprintf(buffer, "Linear");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	if (W3D_GetState(context,W3D_TEXMAPPING) == W3D_ENABLED)
		sprintf(buffer, "Texture Mapping");
	else
		sprintf(buffer, "Gouraud Shading");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	switch(CurrentBlend) {
	case 0:
		sprintf(buffer, "Blend REPLACE");
		break;
	case 1:
		sprintf(buffer, "Blend DECAL");
		break;
	case 2:
		sprintf(buffer, "Blend MODULATE");
		break;
	case 3:
		sprintf(buffer, "Blend BLEND");
		break;
	default:
		sprintf(buffer, "Blend UNKNOWN");
	}
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	sprintf(buffer, "theta=%3.1f", theta);
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	sprintf(buffer, "pos=(%3.1f %3.1f %3.1f)", Pos.x, Pos.y, Pos.z);
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	sprintf(buffer, "Z-Clipping Plane %3.2f", zdepth);
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;
	{
		char *zm;
		switch(zmode) {
		case 0:             zm = "Z-Buffer disabled";   break;
		case W3D_Z_NEVER:   zm = "Z-Buffer never pass"; break;
		case W3D_Z_LESS:    zm = "Z-Buffer less";       break;
		case W3D_Z_GEQUAL:  zm = "Z-Buffer greater/equal"; break;
		case W3D_Z_LEQUAL:  zm = "Z-Buffer less/equal"; break;
		case W3D_Z_GREATER: zm = "Z-Buffer greater";    break;
		case W3D_Z_NOTEQUAL:zm = "Z-Buffer not equal";  break;
		case W3D_Z_EQUAL:   zm = "Z-Buffer equal";      break;
		case W3D_Z_ALWAYS:  zm = "Z-Buffer always pass";break;
		default:            zm = "??? Unknown Z-Mode";  break;
		}
		Move(rp, x,y); Text(rp, zm, strlen(zm)); y+=14;
	}

	sprintf(buffer, "Last Z: %3.2f", LastZ);
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	if (W3D_GetState(context,W3D_BLENDING) == W3D_ENABLED)
		sprintf(buffer, "Alpha Blending");
	else
		sprintf(buffer, "No Blending");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	switch (CurrentFog) {
		case 0: sprintf(buffer, "Fog off");             break;
		case 1: sprintf(buffer, "Fog linear");          break;
		case 2: sprintf(buffer, "Fog exponential");     break;
		case 3: sprintf(buffer, "Fog exponential 2");   break;
		case 4: sprintf(buffer, "Fog interpolated");    break;
	}
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	sprintf(buffer, "(%3.2f - %3.2f)",
		context->fog.fog_start, context->fog.fog_end);
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	switch(curfilt) {
		case 1: sprintf(buffer, "W3D_NEAREST"); break;
		case 2: sprintf(buffer, "W3D_LINEAR"); break;
		case 3: sprintf(buffer, "W3D_NEAREST_MIP_NEAREST"); break;
		case 4: sprintf(buffer, "W3D_LINEAR_MIP_LINEAR"); break;
	}
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

	y=430;
	x=10;

	sprintf(buffer, "A - Mipmap view  B - Vertex coords  C - Slow Motion  P - Perspective Mapping");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;
	sprintf(buffer, "T - Texture Mapping  F - Fogging  L - Alpha  1..4 - Filter");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;
	sprintf(buffer, "E - Blendmode  ESC - Quit");
	Move(rp, x,y); Text(rp, buffer, strlen(buffer)); y+=14;

}

;;//
;// ClearWindow
void ClearWindow(W3D_Context *context, struct RastPort *drawrp)
{
	int i,j,k;
	int w = tex->texwidth;
	int h = tex->texheight;

	EraseRect(drawrp, 0, 0, 639, 479);

	if (dflag) {
		int blah = 0;
		SetAPen(drawrp,10);
		for (i=30; i<480; i+=30) {
			Move(drawrp, 0, i+blah);
			Draw(drawrp, 639, i+blah);
		}
		for (i=30; i<640; i+=30) {
			Move(drawrp, i, blah);
			Draw(drawrp, i, blah+479);
		}
	}
	if (aflag) {
		// If AFlag is set, draw the mipmap
		WritePixelArray(tex->texsource, 0,0, w*4,
			drawrp, 0,0, w,h, RECTFMT_ARGB);
		if (w>h) {
			j = w/2;
			k = w;
		} else{
			j = h/2;
			k = w;
		}

		i=0;
		while (j) {
			WritePixelArray(tex->mipmaps[i++],0,0,w*2,
				drawrp,
				k,0, w/2,h/2, RECTFMT_ARGB);
			k+=w/2;
			j/=2;
			w/=2; if (w == 0) w = 1;
			h/=2; if (h == 0) h = 1;
		}
	}
}

;;//
;// DrawWall
void DrawWall(W3D_Context *context, struct RastPort *drawrp)
{
	static char buffer[512];
	int i;

	Transform(Pos, theta);    // Do the perspective transformations
#if 1
	if (outline) {
		static W3D_Color col[4] = {
			{0.9, 0.9, 0.0, 1.0},
			{0.9, 0.0, 0.1, 0.2},
			{0.0, 0.0, 0.9, 0.5},
			{0.9, 0.9, 0.9, 1.0}
		};
		static W3D_Lines temp;
		static W3D_Vertex v[4];
		for(i = 0; i < 4; i++) {
			v[i].x = TempSquare[i].x;
			v[i].y = TempSquare[i].y;
			v[i].z = TempSquare[i].z;    //  Needed for zbuffer
			v[i].w = TempSquare[i].iz;   //  Needed for fogging
			v[i].color.r = col[i].r;
			v[i].color.g = col[i].g;
			v[i].color.b = col[i].b;
			v[i].color.a = col[i].a;
		}
		temp.v = v;
		temp.vertexcount = 4;
		W3D_DrawLineLoop(context, &temp);
	}
#endif
	tri.tex = tex;  // Set the texture handle

   /*
	A few notes about the triangle structure:

	The "x" and "y" components are pretty obvious. They specify the screen
	coordinates of the corresponding vertex (as a float; this gives you
	sub-pixel accuracy, which is especially useful for small triangles).

	The "z" component is the depth of the vertex. 0.0 means this vertex
	sticks to the glass of your monitor (i.e. is closest to the front
	clipping plane), while 1.0 means that the vertex is far away. The "z"
	component is used for Z-Buffering, and, depending on the driver, might
	be used for fogging as well

	"u" and "v" are the texture coordinates on this vertex, expressed
	as pixels on the map. Note that this conforms with the OpenGL notion
	of "u" and "v". "u" and "v" are not to be confused with OpenGL's
	"s" and "t", which are in the range [0..1].

	color is the vertex color if lighting is used. Note that the alpha
	value might or might not be used, depending on the blending mode.
	The ViRGE implementation ignores this value, since the vertex alpha
	is required for fogging.

	Note however, that the texture alpha is always taken into account (if
	the required blending mode is selected)
	*/

	tri.v1.x = TempSquare[3].x; tri.v1.y = TempSquare[3].y; tri.v1.z = TempSquare[3].z;
	tri.v1.w = TempSquare[3].iz;        //1.f-tri.v1.z;
	tri.v1.u = Square[3].u;     tri.v1.v = Square[3].v;
	tri.v1.color.r = 1.0; tri.v1.color.g = 1.0; tri.v1.color.b = 1.0; tri.v1.color.a = 1.0;

	tri.v2.x = TempSquare[1].x; tri.v2.y = TempSquare[1].y; tri.v2.z = TempSquare[1].z;
	tri.v2.w = TempSquare[1].iz;        //1.f-tri.v2.z;
	tri.v2.u = Square[1].u;     tri.v2.v = Square[1].v;
	tri.v2.color.r = 1.0; tri.v2.color.g = 0.0; tri.v2.color.b = 0.0; tri.v2.color.a = 0.5;

	tri.v3.x = TempSquare[0].x; tri.v3.y = TempSquare[0].y; tri.v3.z = TempSquare[0].z;
	tri.v3.w = TempSquare[0].iz;        //1.f-tri.v3.z;
	tri.v3.u = Square[0].u;     tri.v3.v = Square[0].v;
	tri.v3.color.r = 1.0; tri.v3.color.g = 1.0; tri.v3.color.b = 0.1; tri.v3.color.a = 1.0;

	// You must lock the hardware prior to drawing anything.
	// Note that AUTOLOCKING, if enabled, will do this for you,
	// but it is strictly recommended that you do locking yourself,
	// since performance will suffer if you lock the hardware for
	// every triangle. It is more efficient to lock hardware once for
	// a few triangles, for example, for one polygon.
	//
	// NOTE 2: The CyberGfx implementation of the graphic sub-system
	// driver uses LockBitmap() for this, so you may not hold the
	// lock longer than one display frame (i.e. for a short time)
	if (W3D_SUCCESS != W3D_LockHardware(context)) {
//        printf("Error: Can`t lock hardware\n");
		return;
	}


	// Go ahead and fire off the first triangle.
	W3D_DrawTriangle(context, &tri);

	// Fill in the triangle structure again.
	tri.v1.x = TempSquare[1].x; tri.v1.y = TempSquare[1].y; tri.v1.z = TempSquare[1].z;
	tri.v1.w = TempSquare[1].iz;        //1.f-tri.v1.z;
	tri.v1.u = Square[1].u;     tri.v1.v = Square[1].v;

	tri.v2.x = TempSquare[2].x; tri.v2.y = TempSquare[2].y; tri.v2.z = TempSquare[2].z;
	tri.v2.w = TempSquare[2].iz;        //1.f-tri.v2.z;
	tri.v2.u = Square[2].u;     tri.v2.v = Square[2].v;

	tri.v3.x = TempSquare[3].x; tri.v3.y = TempSquare[3].y; tri.v3.z = TempSquare[3].z;
	tri.v3.w = TempSquare[3].iz;        //1.f-tri.v3.z;
	tri.v3.u = Square[3].u;     tri.v3.v = Square[3].v;

	tri.v1.color.r = 1.0; tri.v1.color.g = 0.0; tri.v1.color.b = 0.0; tri.v1.color.a = 0.5;
	tri.v2.color.r = 0.0; tri.v2.color.g = 0.0; tri.v2.color.b = 1.0; tri.v2.color.a = 0.1;
	tri.v3.color.r = 1.0; tri.v3.color.g = 1.0; tri.v3.color.b = 1.0; tri.v3.color.a = 1.0;

	// Fire off the second triangle. Note that the drawing routines
	// will wait before writing the values to the registers, but
	// if the W3D_SYNCHRON state is not set, it will return immediately
	// and not wait for completion of the operation. In this case
	// it is your duty to wait until the hardware is idle again.
	W3D_DrawTriangle(context, &tri);

	if (result[3]) {
		ULONG Blend;
		W3D_SetBlendMode(context, W3D_SRC_ALPHA, W3D_ONE_MINUS_SRC_ALPHA);

		Blend = W3D_GetState(context, W3D_BLENDING);
		W3D_SetState(context, W3D_BLENDING, W3D_ENABLE);

		tri.tex = lighttex;  // Set the texture handle
		tri.v1.x = TempSquare[3].x; tri.v1.y = TempSquare[3].y; tri.v1.z = TempSquare[3].z;
		tri.v1.w = TempSquare[3].iz;        //1.f-tri.v1.z;
		tri.v1.u = LSquare[3].u;     tri.v1.v = LSquare[3].v;
		tri.v1.color.r = 1.0; tri.v1.color.g = 1.0; tri.v1.color.b = 1.0; tri.v1.color.a = 1.0;

		tri.v2.x = TempSquare[1].x; tri.v2.y = TempSquare[1].y; tri.v2.z = TempSquare[1].z;
		tri.v2.w = TempSquare[1].iz;        //1.f-tri.v2.z;
		tri.v2.u = LSquare[1].u;     tri.v2.v = LSquare[1].v;
		tri.v2.color.r = 1.0; tri.v2.color.g = 1.0; tri.v2.color.b = 1.0; tri.v2.color.a = 1.0;

		tri.v3.x = TempSquare[0].x; tri.v3.y = TempSquare[0].y; tri.v3.z = TempSquare[0].z;
		tri.v3.w = TempSquare[0].iz;        //1.f-tri.v3.z;
		tri.v3.u = LSquare[0].u;     tri.v3.v = LSquare[0].v;
		tri.v3.color.r = 1.0; tri.v3.color.g = 1.0; tri.v3.color.b = 1.0; tri.v3.color.a = 1.0;

		W3D_DrawTriangle(context, &tri);

		tri.v1.x = TempSquare[1].x; tri.v1.y = TempSquare[1].y; tri.v1.z = TempSquare[1].z;
		tri.v1.w = TempSquare[1].iz;        //1.f-tri.v1.z;
		tri.v1.u = LSquare[1].u;     tri.v1.v = LSquare[1].v;

		tri.v2.x = TempSquare[2].x; tri.v2.y = TempSquare[2].y; tri.v2.z = TempSquare[2].z;
		tri.v2.w = TempSquare[2].iz;        //1.f-tri.v2.z;
		tri.v2.u = LSquare[2].u;     tri.v2.v = LSquare[2].v;

		tri.v3.x = TempSquare[3].x; tri.v3.y = TempSquare[3].y; tri.v3.z = TempSquare[3].z;
		tri.v3.w = TempSquare[3].iz;        //1.f-tri.v3.z;
		tri.v3.u = LSquare[3].u;     tri.v3.v = LSquare[3].v;

		tri.v1.color.r = 1.0; tri.v1.color.g = 1.0; tri.v1.color.b = 1.0; tri.v1.color.a = 1.0;
		tri.v2.color.r = 1.0; tri.v2.color.g = 1.0; tri.v2.color.b = 1.0; tri.v2.color.a = 1.0;
		tri.v3.color.r = 1.0; tri.v3.color.g = 1.0; tri.v3.color.b = 1.0; tri.v3.color.a = 1.0;

		W3D_DrawTriangle(context, &tri);
		W3D_SetState(context, W3D_BLENDING, Blend);
	}

	if (second_wall) {
		float t2;

		t2 = 360.0 - theta;
		if (t2 < 0) t2 = 360.0 - t2;

		Transform(Pos, t2);    // Do the perspective transformations

		tri.tex = tex;  // Set the texture handle

		tri.v1.x = TempSquare[3].x; tri.v1.y = TempSquare[3].y; tri.v1.z = TempSquare[3].z;
		tri.v1.w = TempSquare[3].iz;        //1.f-tri.v1.z;
		tri.v1.u = Square[3].u;     tri.v1.v = Square[3].v;
		tri.v1.color.r = 1.0; tri.v1.color.g = 1.0; tri.v1.color.b = 1.0; tri.v1.color.a = 1.0;

		tri.v2.x = TempSquare[1].x; tri.v2.y = TempSquare[1].y; tri.v2.z = TempSquare[1].z;
		tri.v2.w = TempSquare[1].iz;        //1.f-tri.v2.z;
		tri.v2.u = Square[1].u;     tri.v2.v = Square[1].v;
		tri.v2.color.r = 1.0; tri.v2.color.g = 0.0; tri.v2.color.b = 0.0; tri.v2.color.a = 1.0;

		tri.v3.x = TempSquare[0].x; tri.v3.y = TempSquare[0].y; tri.v3.z = TempSquare[0].z;
		tri.v3.w = TempSquare[0].iz;        //1.f-tri.v3.z;
		tri.v3.u = Square[0].u;     tri.v3.v = Square[0].v;
		tri.v3.color.r = 1.0; tri.v3.color.g = 1.0; tri.v3.color.b = 0.0; tri.v3.color.a = 1.0;

		// Go ahead and fire off the first triangle.
		W3D_DrawTriangle(context, &tri);

		// Fill in the triangle structure again.
		tri.v1.x = TempSquare[1].x; tri.v1.y = TempSquare[1].y; tri.v1.z = TempSquare[1].z;
		tri.v1.w = TempSquare[1].iz;        //1.f-tri.v1.z;
		tri.v1.u = Square[1].u;     tri.v1.v = Square[1].v;

		tri.v2.x = TempSquare[2].x; tri.v2.y = TempSquare[2].y; tri.v2.z = TempSquare[2].z;
		tri.v2.w = TempSquare[2].iz;        //1.f-tri.v2.z;
		tri.v2.u = Square[2].u;     tri.v2.v = Square[2].v;

		tri.v3.x = TempSquare[3].x; tri.v3.y = TempSquare[3].y; tri.v3.z = TempSquare[3].z;
		tri.v3.w = TempSquare[3].iz;        //1.f-tri.v3.z;
		tri.v3.u = Square[3].u;     tri.v3.v = Square[3].v;

		tri.v1.color.r = 1.0; tri.v1.color.g = 0.0; tri.v1.color.b = 0.0; tri.v1.color.a = 1.0;
		tri.v2.color.r = 0.0; tri.v2.color.g = 0.0; tri.v2.color.b = 1.0; tri.v2.color.a = 1.0;
		tri.v3.color.r = 1.0; tri.v3.color.g = 1.0; tri.v3.color.b = 1.0; tri.v3.color.a = 1.0;

		// Fire off the second triangle. Note that the drawing routines
		// will wait before writing the values to the registers, but
		// if the W3D_SYNCHRON state is not set, it will return immediately
		// and not wait for completion of the operation. In this case
		// it is your duty to wait until the hardware is idle again.
		W3D_DrawTriangle(context, &tri);

		if (outline) {
			W3D_Color col[4] = {
				{0.9, 0.9, 0.0, 1.0},
				{0.9, 0.0, 0.0, 1.0},
				{0.0, 0.0, 0.9, 1.0},
				{0.9, 0.9, 0.9, 1.0}
			};
			W3D_Line temp;
			temp.v1.color.a = 1.0;
			temp.v2.color.a = 0.2;

			for(i = 0; i < 4; i++) {
				temp.v1.x = TempSquare[i].x;
				temp.v1.y = TempSquare[i].y;
				temp.v1.z = TempSquare[i].z;    //  Needed for zbuffer
				temp.v1.w = TempSquare[i].iz;   //  Needed for fogging
				temp.v2.x = TempSquare[(i+1)%4].x;
				temp.v2.y = TempSquare[(i+1)%4].y;
				temp.v2.z = TempSquare[(i+1)%4].z;
				temp.v2.w = TempSquare[(i+1)%4].iz;
				temp.v1.color.r = col[i].r;
				temp.v1.color.g = col[i].g;
				temp.v1.color.b = col[i].b;
				temp.v2.color.r = col[(i+1)%4].r;
				temp.v2.color.g = col[(i+1)%4].g;
				temp.v2.color.b = col[(i+1)%4].b;
				W3D_DrawLine(context, &temp);
			}
		}
	}

	W3D_UnLockHardware(context);

	SetAPen(drawrp, 249);
	RectFill(drawrp, 638,0,639,1);

	// If the user flipped the bflag on, draw the coordinates at the
	// polygon vertices
	if (bflag) {
		SetAPen(drawrp, 255);
		for (i=0; i<4; i++) {
			Move(drawrp, (long)TempSquare[i].x, (long)TempSquare[i].y);
			sprintf(buffer, "%3.2f,%3.2f", TempSquare[i].z, TempSquare[i].iz);
			Text(drawrp, buffer, strlen(buffer));
		}
	}
}

;;//
;// SwitchBuffer
void SwitchBuffer(W3D_Context *context, struct RastPort *drawrp, struct RastPort *windowrp, BOOL clip)
{
	W3D_Scissor s = {0, 0, 640, 480};

	if (clip) {
		s.left = 10;
		s.top = 20;
		s.width = 400;
		s.height = 300;
	}

	PrintInfo(context, drawrp);

	W3D_WaitIdle(context);
	W3D_FlushFrame(context);	/* force any software driver to finish drawing */

	BltBitMapRastPort(drawrp->BitMap, 0, 0, windowrp, 0, 0, 640, 480, 0xC0);

	W3D_SetDrawRegion(context, drawrp->BitMap, 0, &s);
	if (clip)   W3D_SetScissor(context, &s);
}

;;//
;// PrintDriverInfo
void PrintDriverInfo(void)
{
	ULONG res;

	W3D_Driver **drivers = W3D_GetDrivers();

	if (*drivers == NULL) {
		printf("Panic: No Drivers found\n");
		return;
	}
	printf("Available drivers:\n");
	while (*drivers) {
		printf("%s\n\tSupports format 0x%X\n\t",
			drivers[0]->name, (unsigned int)drivers[0]->formats);
		if (drivers[0]->swdriver) printf("CPU Driver\n");
		else                      printf("Hardware Driver\n");

		printf("\tPrimitives supported:\n\t");
		res = W3D_QueryDriver(drivers[0], W3D_Q_DRAW_POINT, W3D_FMT_R5G5B5);
		if (res != W3D_NOT_SUPPORTED) printf("[POINT] ");

		res = W3D_QueryDriver(drivers[0], W3D_Q_DRAW_LINE, W3D_FMT_R5G5B5);
		if (res != W3D_NOT_SUPPORTED) printf("[LINE] ");

		res = W3D_QueryDriver(drivers[0], W3D_Q_DRAW_TRIANGLE, W3D_FMT_R5G5B5);
		if (res != W3D_NOT_SUPPORTED) printf("[TRIANGLE] ");

		printf("\n\tFiltering:\n\t");
		res = W3D_QueryDriver(drivers[0], W3D_Q_BILINEARFILTER, W3D_FMT_R5G5B5);
		if (res != W3D_NOT_SUPPORTED) printf("[BI-FILTER] ");

		res = W3D_QueryDriver(drivers[0], W3D_Q_MMFILTER, W3D_FMT_R5G5B5);
		if (res != W3D_NOT_SUPPORTED) printf("[MM-FILTER] ");
		printf("\n");

		drivers++;
	}
	printf("\n\n");
}
;;//
;// main
int main(int argc, char **argv)
{
	ULONG ModeID;
	ULONG OpenErr, CError, res, ret;
	struct BitMap *bm = NULL;
	struct BitMap *buffer = NULL;
	struct RastPort *drawrp = NULL;
	W3D_Context *context = NULL;
	BOOL running=TRUE, clip = FALSE;
	struct IntuiMessage *imsg;
	ULONG flags;
	UBYTE *newm;
	int si;
	LONG repeat = 1;
	W3D_Scissor s = {0, 0, 640, 480};
	W3D_Scissor sc = {32, 32, 64, 64};

	if (argc != 0)
	{
		rda = (struct RDArgs *)AllocDosObject(DOS_RDARGS, NULL);
		bzero(result, 40);
		if (rda) {
			rda->RDA_Source.CS_Buffer = NULL;
			ReadArgs(TEMPLATE, result, rda);
			FreeDosObject(DOS_RDARGS, rda);
		}
		if (result[2]) repeat = *((LONG *)result[2]);
		else           repeat = 1;
	}


	// Initialize the required libraries (We assume intuition and graphics open OK
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0L);
	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0L);


	CyberGfxBase = OpenLibrary("cybergraphics.library", 0L);
	if (!CyberGfxBase) {
		printf("Error opening CyberGraphX library\n");
		goto panic;
	}

	Warp3DBase = OpenLibrary("Warp3D.library", 2L);
	if (!Warp3DBase) {
		printf("Error opening Warp3D library\n");
		goto panic;
	}

	// Check for availability of drivers
	flags = W3D_CheckDriver();
	if (flags & W3D_DRIVER_3DHW) printf("Hardware driver available\n");
	if (flags & W3D_DRIVER_CPU)  printf("Software driver available\n");
	if (flags & W3D_DRIVER_UNAVAILABLE) {
		printf("PANIC: no driver available!!!\n");
		goto panic;
	}

	PrintDriverInfo();

	/*
	** New in V2: The Screenmode requester
	** This requester will ask for any screenmode the installed
	** Hardware driver can handle, and also restrict the presented
	** screen modes to 640x480 Modes...
	*/
	ModeID = W3D_RequestModeTags(
		W3D_SMR_TYPE,       W3D_DRIVER_3DHW,
		W3D_SMR_SIZEFILTER, TRUE,
		W3D_SMR_DESTFMT,    ~W3D_FMT_CLUT,
		ASLSM_MinWidth,     640,
		ASLSM_MaxWidth,     641,
		ASLSM_MinHeight,    480,
		ASLSM_MaxHeight,    481,
	TAG_DONE);

	if (ModeID == INVALID_ID) {
		printf("Error: No ModeID found\n");
		goto panic;
	}

	// Open Screen
	screen = OpenScreenTags(NULL,
		//SA_Height,    640,
		SA_DisplayID, ModeID,
		SA_Depth,     8,
		SA_ErrorCode, (IPTR)&OpenErr,
		SA_ShowTitle, FALSE,
		SA_Draggable, FALSE,
	TAG_DONE);

	if (!screen) {
		printf("Unable to open screen. Reason: Error code %d\n", (unsigned int)OpenErr);
		goto panic;
	}

	// Open window
	// While this is not strictly nessessary, we use it because
	// we want to get IDCMP messages. You can also use the screen's
	// bitmap to render
	window = OpenWindowTags(NULL,
		WA_CustomScreen,    (IPTR)screen,
		WA_Activate,        TRUE,
		WA_Width,           screen->Width,
		WA_Height,          screen->Height,
		WA_Left,            0,
		WA_Top,             0,
		WA_Title,           NULL,
		WA_CloseGadget,     FALSE,
		WA_Backdrop,        TRUE,
		WA_Borderless,      TRUE,
		WA_IDCMP,           IDCMP_CLOSEWINDOW|IDCMP_VANILLAKEY|IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_DELTAMOVE,
		WA_Flags,           WFLG_REPORTMOUSE|WFLG_RMBTRAP,
	TAG_DONE);

	SetRGB32(&(screen->ViewPort), 0, 0x40ffffff, 0x40ffffff, 0x88ffffff);
	SetRGB32(&(screen->ViewPort), 10, 0xffffffff, 0x0, 0x0);

	if (!window) {
		printf("Unable to open window.\n");
		goto panic;
	}

	bm = window->RPort->BitMap;

	SetAPen(window->RPort, 249);
	RectFill(window->RPort, 0, 0, 639, 479);

	buffer = AllocBitMap(screen->Width, screen->Height, 8, BMF_DISPLAYABLE|BMF_MINPLANES, bm);
	if (!buffer) {
		printf("Unable to create buffer bitmap.\n");
		goto panic;
	}
	
	// We want to use this bitmap
	drawrp = CreateRastPort();
	if (!drawrp) {
		printf("Unable to create rastport.\n");
		goto panic;
	}
	
	drawrp->BitMap = buffer;

	// Go ahead and create the context. We need a context for every drawing
	// operation, so this is done quite early in the program.
	// NOTE: Some functions, like W3D_Query, do work without a context.
	// The W3D_Query function without a context can be used to determine
	// what kind of display/texture format the chip driver prefers.

	context = W3D_CreateContextTags(&CError,
		W3D_CC_MODEID,      ModeID,             // Mandatory for non-pubscreen
		W3D_CC_BITMAP,      (IPTR)buffer,       // The bitmap we'll use
		W3D_CC_YOFFSET,     0,                  // We don't do dbuffering
		W3D_CC_DRIVERTYPE,  W3D_DRIVER_BEST,    // Let Warp3D decide
// I had the change the following to FALSE to make it work with AROS		
		W3D_CC_DOUBLEHEIGHT,FALSE,              // Double height screen
		W3D_CC_FAST,        TRUE,               // Fast drawing
	TAG_DONE);

	if (!context || CError != W3D_SUCCESS) {
		printf("Error creating context. Reason:");
		switch(CError) {
		case W3D_ILLEGALINPUT:
			printf("Illegal input to CreateContext function\n");
			break;
		case W3D_NOMEMORY:
			printf("Out of memory\n");
			break;
		case W3D_NODRIVER:
			printf("No suitable driver found\n");
			break;
		case W3D_UNSUPPORTEDFMT:
			printf("Supplied bitmap cannot be handled by Warp3D\n");
			break;
		case W3D_ILLEGALBITMAP:
			printf("Supplied bitmap not properly initialized\n");
			break;
		default:
			printf("An error has occured... gosh\n");
		}
		goto panic;
	}

	if (result[0]) {
		if (FALSE == GenTexture(context, (char *)result[0], repeat)) {
			printf("Error loading/generating texture\n");
			goto panic;
		}
	} else {
		if (FALSE == GenTexture(context, "wall.ppm", repeat)) {
			printf("Error loading/generating default texture\n");
			goto panic;
		}
	}

	if (result[3]) {
		if (FALSE == GenLightmap(context, (char *)result[3], repeat)) {
			printf("Error loading/generating lightmap\n");
			goto panic;
		}
	}

 /* If you need to know the "success" of setting a state, you can
	find out by examining the return value of W3D_SetState. This should
	be done if your program depends heavily on this feature. If not, you
	may safely ignore the result. For example, the W3D_DITHERING state
	might not have a large impact on the performance of your engine, so
	you might ignore the outcome. On the other hand, it might be a severe
	problem if your driver does not support texture mapping.

	Things like this can be verified with W3D_Query after the context was
	created, or with W3D_QueryDriver if you didn't create a context yet.
 */

	printf("Setting states\n");

	// Set dithering. This will look considerably better on 15 bit displays.
	W3D_SetState(context, W3D_DITHERING, W3D_ENABLE);
	// Set Gouraud shading state
	W3D_SetState(context, W3D_GOURAUD, W3D_ENABLE);
	// Set texture wrapping mode to on.
	// Read the section about limitations of the Virge for this
	W3D_SetWrapMode(context, tex, W3D_REPEAT, W3D_REPEAT, NULL);
	// Set blending to CurrentBlend mode (defaults to no alpha blending)
	W3D_SetTexEnv(context, tex, BlendModes[CurrentBlend], NULL);
	// Set Fog parameters
	W3D_SetFogParams(context, &myFog, FogModes[CurrentFog]);
	// Set Texture mapping
	W3D_SetState(context, W3D_TEXMAPPING, W3D_ENABLE);
	// Set Drawing region
	W3D_SetDrawRegion(context, buffer, 0, &s);
	// Set current color
	W3D_SetCurrentColor(context, &ccol);
	if (result[1]) {
		// Set ZBuffer modes
		if (W3D_SUCCESS != W3D_AllocZBuffer(context)) {
			printf("Error: Can`t create ZBuffer\nZBuffering will not be available\n");
		} else {
			zb = TRUE;
			W3D_SetState(context, W3D_ZBUFFERUPDATE, W3D_ENABLE);
			if (W3D_SUCCESS == W3D_LockHardware(context)) {
				W3D_ClearZBuffer(context, &zdepth);
				W3D_UnLockHardware(context);
			}
		}
	}

	printf("Going into main loop\n");

	// Clear window, then draw our first wall.
	ClearWindow(context, drawrp);
	DrawWall(context, drawrp);

	running=TRUE;

	while (running) {
//        WaitPort(window->UserPort);
		while ((imsg = (struct IntuiMessage *)GetMsg(window->UserPort))) {
			if (imsg == NULL) break;
			switch(imsg->Class) {
			case IDCMP_MOUSEBUTTONS:
				if (imsg->Code == SELECTDOWN) {
					drag = 1;
				}
				if (imsg->Code == SELECTUP) {
					drag = 0;
				}
				if (imsg->Code == MENUDOWN) {
					push = 1;
				}
				if (imsg->Code == MENUUP) {
					push = 0;
				}
				break;
			case IDCMP_MOUSEMOVE:
				if (drag) {
					theta += (float)(imsg->MouseX);
					if (theta < 0.0) theta += 360.f;
					if (theta > 360.0) theta -= 360.f;
				}
				if (push) {
					Pos.z += (float)(imsg->MouseY)/128.f;
					if (Pos.z < Front+1.f) Pos.z = Front+1.f;
					if (Pos.z > Back-1.f) Pos.z = Back-1.f;
				}
				break;
			case IDCMP_CLOSEWINDOW:
				running=FALSE;
				break;
			case IDCMP_RAWKEY:
				if (cflag == 0) {
					switch(imsg->Code) {
					case 0x4F:
						theta -= 4.f;
						if (theta < 0.0) theta += 360.f;
						break;
					case 0x4e:
						theta += 4.f;
						if (theta > 360.0) theta -= 360.f;
						break;
					case 0x4C:
						Pos.z -= 1.f;
						if (Pos.z < Front+1.f) Pos.z = Front+1.f;
						break;
					case 0x4D:
						Pos.z += 1.f;
						if (Pos.z > Back-1.f) Pos.z = Back-1.f;
						break;
					case 0x50:
						W3D_SetWrapMode(context, tex, W3D_CLAMP, W3D_CLAMP, NULL);
						break;
					case 0x51:
						W3D_SetWrapMode(context, tex, W3D_REPEAT, W3D_REPEAT, NULL);
						break;
					case 0x5f:      // Debug key
						clip = !clip;
						if (clip) W3D_SetState(context, W3D_SCISSOR, W3D_ENABLE);
						else      W3D_SetState(context, W3D_SCISSOR, W3D_DISABLE);
						break;
					break;
					}
				} else {
					switch(imsg->Code) {
					case 0x4F:
						theta -= 1.f;
						if (theta < 0.0) theta += 360.f;
						break;
					case 0x4e:
						theta += 1.f;
						if (theta > 360.0) theta -= 360.f;
						break;
					case 0x4C:
						Pos.z -= 0.25f;
						if (Pos.z < Front+1.f) Pos.z = Front+1.f;
						break;
					case 0x4D:
						Pos.z += 0.25f;
						if (Pos.z > Back-1.f) Pos.z = Back-1.f;
						break;
					}
				}
				break;
			case IDCMP_VANILLAKEY:
				switch(imsg->Code) {
				case '|':
					newm = LoadTextureFromPPM((APTR)NULL, (char *)"smallugly.ppm", 30, &si, &si);
					W3D_UpdateTexSubImage(context, tex, (void *)newm, 0, NULL, &sc, 0);
					FreeVec(newm);
					break;
				case 'S':
					second_wall = 1 - second_wall;
					break;
				case '8':   // Move fog end further back
					myFog.fog_end -= 0.01;
					if (myFog.fog_end < 0.05) myFog.fog_end = 0.05;
					W3D_SetFogParams(context, &myFog, FogModes[CurrentFog]);
					break;
				case '5':   // Move fog end forward
					myFog.fog_end += 0.01;
					if (myFog.fog_end+0.05 > myFog.fog_start) myFog.fog_end = myFog.fog_start-0.05;
					W3D_SetFogParams(context, &myFog, FogModes[CurrentFog]);
					break;
				case '9':   // Move fog start further back
					myFog.fog_start -= 0.01;
					if (myFog.fog_start-0.05 < myFog.fog_end) myFog.fog_start = myFog.fog_end+0.05;
					W3D_SetFogParams(context, &myFog, FogModes[CurrentFog]);
					break;
				case '6':   // Move fog start forward
					myFog.fog_start += 0.01;
					if (myFog.fog_start > 1.0) myFog.fog_start = 1.0;
					W3D_SetFogParams(context, &myFog, FogModes[CurrentFog]);
					break;
				case 'F': // Toggle fog quality
					if (FogQuality == W3D_H_NICE) FogQuality = W3D_H_FAST;
					else                          FogQuality = W3D_H_NICE;
					W3D_Hint(context, W3D_H_FOGGING, FogQuality);
					break;
				case 't':
				case 'T':   // Toggle texture mapping
					if (W3D_GetState(context, W3D_TEXMAPPING) == W3D_ENABLED)
						W3D_SetState(context, W3D_TEXMAPPING, W3D_DISABLE);
					else
						W3D_SetState(context, W3D_TEXMAPPING, W3D_ENABLE);
					break;
				case 'p':
				case 'P':   // Toggle perspective mapping
					if (forcelin == 1) break; // Exit if forced to linear
					if (persp==0) persp=1;
					else          persp=0;
					if (persp) W3D_SetState(context, W3D_PERSPECTIVE, W3D_ENABLE);
					else       W3D_SetState(context, W3D_PERSPECTIVE, W3D_DISABLE);
					break;
				case '1':
					curfilt = 1;
					ret = W3D_SetFilter(context, tex, W3D_NEAREST, W3D_NEAREST);
					if (ret != W3D_SUCCESS) printf("Warning: Unsupported Filter mode\n");
					if (lighttex) W3D_SetFilter(context, tex, W3D_NEAREST, W3D_NEAREST);
					break;
				case '2':
					curfilt = 2;
					ret = W3D_SetFilter(context, tex, W3D_LINEAR, W3D_LINEAR);
					if (ret != W3D_SUCCESS) printf("Warning: Unsupported Filter mode\n");
					if (lighttex) W3D_SetFilter(context, tex, W3D_LINEAR, W3D_LINEAR);
					break;
				case '3':
					curfilt = 3;
					ret = W3D_SetFilter(context, tex, W3D_NEAREST_MIP_NEAREST, W3D_NEAREST);
					if (ret != W3D_SUCCESS) printf("Warning: Unsupported Filter mode\n");
					if (lighttex) W3D_SetFilter(context, tex, W3D_NEAREST_MIP_NEAREST, W3D_NEAREST);
					break;
				case '4':
					curfilt = 4;
					ret = W3D_SetFilter(context, tex, W3D_LINEAR_MIP_LINEAR, W3D_LINEAR);
					if (ret != W3D_SUCCESS) printf("Warning: Unsupported Filter mode\n");
					if (lighttex) W3D_SetFilter(context, tex, W3D_LINEAR_MIP_LINEAR, W3D_LINEAR);
					break;
				case 'a':
				case 'A':   // Toggle "Show mipmaps" (A Flag)
					pixels = 1-pixels;
					break;
				case 'b':
				case 'B':   // Toggle "Show coordinates" (B Flag)
					bflag = 1-bflag;
					break;
				case 'c':
				case 'C':   // Toggle "Slowmotion" (C FLag)
					cflag = 1-cflag;
					break;
				case 'D':   // Debug stuff. Don't look here, it's nasty
					if (W3D_SUCCESS == W3D_LockHardware(context))
					{
						W3D_ClearDrawRegion(context, 0);
						W3D_UnLockHardware(context);
					}
					break;
				case 'd':
					dflag = 1-dflag;
					break;
				case 'e':
				case 'E':   // Toggle Lighting
					do {
						CurrentBlend = (CurrentBlend+1)%4;
						res = W3D_SetTexEnv(context, tex, BlendModes[CurrentBlend], NULL);
					} while (res != W3D_SUCCESS);
					break;
				case 'f': // Toggle fog. Note we ignore success/failure
					do {
						CurrentFog = (CurrentFog+1)%5;
						if (CurrentFog == 0) {
							res = W3D_SetState(context, W3D_FOGGING, W3D_DISABLE);
						} else {
							res = W3D_SetFogParams(context, &myFog, FogModes[CurrentFog]);
							if (res == W3D_SUCCESS)
								W3D_SetState(context, W3D_FOGGING, W3D_ENABLE);
						}
					} while (res != W3D_SUCCESS);
					break;
				case 'o':
				case 'O':
					outline = ~outline;
					break;
				case 'z':
				case 'Z':   // Toggle ZBuffer. Currently not supported
					zmode++; if (zmode == 9) zmode=0;
					if (zmode==0) {
						W3D_SetState(context, W3D_ZBUFFER, W3D_DISABLE);
					} else {
						W3D_SetState(context, W3D_ZBUFFER, W3D_ENABLE);
						if (W3D_DISABLED == W3D_GetState(context, W3D_ZBUFFER)) {
							printf("Warning: Can`t enable ZBuffer\n");
						}
						W3D_SetZCompareMode(context, zmode);
					}
					break;
				case 'L':
					if (W3D_GetState(context, W3D_GOURAUD) == W3D_ENABLED)
						W3D_SetState(context, W3D_GOURAUD, W3D_DISABLE);
					else
						W3D_SetState(context, W3D_GOURAUD, W3D_ENABLE);
					break;
				case 'l':   // Toggle Alpha Blending
					W3D_SetBlendMode(context, W3D_SRC_ALPHA, W3D_ONE_MINUS_SRC_ALPHA);
					if (W3D_GetState(context, W3D_BLENDING) == W3D_ENABLED)
						W3D_SetState(context, W3D_BLENDING, W3D_DISABLE);
					else
						W3D_SetState(context, W3D_BLENDING, W3D_ENABLE);
					break;
				case 's':
					if (W3D_GetState(context, W3D_FAST) == W3D_ENABLED)
						W3D_SetState(context, W3D_FAST, W3D_DISABLE);
					else
						W3D_SetState(context, W3D_FAST, W3D_ENABLE);
					break;
				case '+':
					zdepth+=0.05;
					if (zdepth>1.0) zdepth=1.0;
					if (W3D_SUCCESS == W3D_LockHardware(context)) {
						W3D_ClearZBuffer(context, &zdepth);
						W3D_UnLockHardware(context);
					}
					break;
				case '-':
					zdepth-=0.05;
					if (zdepth<0.0) zdepth=0.0;
					if (W3D_SUCCESS == W3D_LockHardware(context)) {
						W3D_ClearZBuffer(context, &zdepth);
						W3D_UnLockHardware(context);
					}
					break;
				case 27:    // Esc or 'Q' quits
				case 'Q':
					running=FALSE;
					break;
				case 13:
					{
					W3D_Double test;
					W3D_ReadZPixel(context, 10, 10, &test);
					printf("Test: %g\n", (double)test);
					}
					break;
				default:
					break;
				}
			break;
			}

			if (imsg) {
				ReplyMsg((struct Message *)imsg);
				imsg = NULL;
			}
		}
		SwitchBuffer(context, drawrp, window->RPort, clip);
		ClearWindow(context, drawrp);
		DrawWall(context, drawrp);
		if (zb && W3D_SUCCESS == W3D_LockHardware(context)) {
			W3D_ClearZBuffer(context, &zdepth);
			W3D_UnLockHardware(context);
		}
	}

panic:
	printf("Closing down...\n");
	if (buffer)         FreeBitMap(buffer);
	if (drawrp)         FreeRastPort(drawrp);
	if (context)        W3D_FreeZBuffer(context);
	if (tex)            W3D_FreeTexObj(context, tex);
	if (lighttex)       W3D_FreeTexObj(context, lighttex);
	if (context)        W3D_DestroyContext(context);
	if (texmap)         FreeVec(texmap);
	if (window)         CloseWindow(window);
	if (screen)         CloseScreen(screen);
	if (Warp3DBase)     CloseLibrary(Warp3DBase);
	if (CyberGfxBase)   CloseLibrary((struct Library *)CyberGfxBase);
	if (IntuitionBase)  CloseLibrary((struct Library *)IntuitionBase);
	if (GfxBase)        CloseLibrary((struct Library *)GfxBase);
	exit(0);
}
;;//
