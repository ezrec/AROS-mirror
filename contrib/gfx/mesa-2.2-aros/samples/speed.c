/*
 * Copyright (c) 1991, 1992, 1993 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the name of
 * Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF
 * ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#define _HPUX_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "gltk.h"

#ifdef __unix
#include <sys/times.h>
#include <sys/param.h>
#endif
#ifdef __bsdi
#include <time.h>
#endif


#define GAP 10
#define ROWS 1
#define COLS 4


GLenum rgb, doubleBuffer, directRender, windType;
GLint windW, windH;

GLint boxW, boxH;

GLenum antialiasing = GL_FALSE;
GLenum depthTesting = GL_FALSE;
GLenum fogging = GL_FALSE, niceFogging = GL_FALSE;
GLenum lighting = GL_FALSE;
GLenum shading = GL_FALSE;
GLenum texturing = GL_FALSE;

GLint repeatCount = 1000;
GLint loopCount = 100;

GLubyte texture[4*3] = {
    0xFF, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0xFF, 0,
};


static void SetWindSize(int width, int height)
{

    windW = (GLint)width;
    windH = (GLint)height;
}

static GLenum Key(int key, GLenum mask)
{

    switch (key) {
      case TK_ESCAPE:
	tkQuit();
      case TK_a:
	antialiasing = !antialiasing;
	break;
      case TK_d:
	depthTesting = !depthTesting;
	break;
      case TK_f:
	fogging = !fogging;
	break;
      case TK_F:
	niceFogging = !niceFogging;
	break;
      case TK_s:
	shading = !shading;
	break;
      case TK_t:
	texturing = !texturing;
	break;
      default:
	return GL_FALSE;
    }
    return GL_TRUE;
}

static void Viewport(GLint row, GLint column)
{
    GLint x, y;

    boxW = (windW - (COLS + 1) * GAP) / COLS;
    boxH = (windH - (ROWS + 1) * GAP) / ROWS;

    x = GAP + column * (boxW + GAP);
    y = GAP + row * (boxH + GAP);

    glViewport(x, y, boxW, boxH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-boxW/2, boxW/2, -boxH/2, boxH/2);
    glMatrixMode(GL_MODELVIEW);

   /* glEnable(GL_SCISSOR_TEST);*/
    glScissor(x, y, boxW, boxH);
}

static double Now(void)
{
#ifdef __unix
    struct tms tm;
#ifdef __MACHTEN__

    times(&tm);

    return (double)tm.tms_utime / (double)CLK_TCK;
#else
    clock_t clk;

    clk = times(&tm);

#ifdef CLK_TCK
    return (double)clk / (double)CLK_TCK;
#else
    return (double)clk / (double)HZ;
#endif
#endif
    return 0;
#endif
}

static void Report(const char *msg, float elapsed)
{

    if (elapsed == 0.0) {
	printf("%s per second: Unknown, elapsed time is zero\n", msg);
    } else {
	printf("%s per second: %g\n", msg, repeatCount*loopCount/elapsed);
    }
}

static void Points(void)
{
    GLint i, j;
    float v1[3];
    double start;

    start = Now();
    for (i = 0; i < repeatCount; i++) {
	v1[0] = 10;
	v1[1] = 10;
	v1[2] = 10;
	glBegin(GL_POINTS);
	    for (j = 0; j < loopCount; j++) {
		glVertex2fv(v1);
	    }
	glEnd();
    }
    glFinish();
    Report("Points", Now()-start);
}

static void Lines(void)
{
    GLint i, j;
    float v1[3], v2[3];
    double start;

    start = Now();
    for (i = 0; i < repeatCount; i++) {
	v1[0] = 10;
	v1[1] = 10;
	v1[2] = 10;
	v2[0] = 20;
	v2[1] = 20;
	v2[2] = 10;
	glBegin(GL_LINES);
	    for (j = 0; j < loopCount; j++) {
		glVertex2fv(v1);
		glVertex2fv(v2);
	    }
	glEnd();
    }
    glFinish();
    Report("Lines", Now()-start);
}

static void Triangles(void)
{
    GLint i, j;
    float v1[3], v2[3], v3[3], t1[2], t2[2], t3[2];
    double start;

    start = Now();

    v1[0] = 10;
    v1[1] = 10;
    v1[2] = 10;
    v2[0] = 20;
    v2[1] = 20;
    v2[2] = 10;
    v3[0] = 10;
    v3[1] = 20;
    v3[2] = 10;

    t1[0] = 0;
    t1[1] = 0;
    t2[0] = 1;
    t2[1] = 1;
    t3[0] = 0;
    t3[1] = 1;

    for (i = 0; i < repeatCount; i++) {
	glBegin(GL_TRIANGLES);
	    for (j = 0; j < loopCount; j++) {
		if (texturing) {
		    glTexCoord2fv(t1);
		}
		glVertex2fv(v1);
		if (texturing) {
		    glTexCoord2fv(t2);
		}
		glVertex2fv(v2);
		if (texturing) {
		    glTexCoord2fv(t3);
		}
		glVertex2fv(v3);
	    }
	glEnd();
    }
    glFinish();
    Report("Triangles", Now()-start);
}

static void Rects(void)
{
    GLint i, j;
    float v1[2], v2[2];
    double start;

    start = Now();
    for (i = 0; i < repeatCount; i++) {
	v1[0] = 10;
	v1[1] = 10;
	v2[0] = 20;
	v2[1] = 20;
	for (j = 0; j < loopCount; j++) {
	    glRectfv(v1, v2);
	}
    }
    glFinish();
    Report("Rects", Now()-start);
}

static void Draw(void)
{

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    TK_SETCOLOR(windType, TK_YELLOW);

    if (antialiasing) {
	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
	glEnable(GL_BLEND);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	printf("antialiasing: on\n");
    }
    else {
       glDisable(GL_BLEND);
       glDisable(GL_POINT_SMOOTH);
       glDisable(GL_LINE_SMOOTH);
       glDisable(GL_POLYGON_SMOOTH);
       printf("antialiasing: off\n");
    }
    if (depthTesting) {
	glEnable(GL_DEPTH_TEST);
	printf("depthtest: on\n");
    }
    else {
       glDisable(GL_DEPTH_TEST);
       printf("depthtest: off\n");
    }
    if (fogging) {
	glEnable(GL_FOG);
	glHint(GL_FOG_HINT, (niceFogging) ? GL_NICEST : GL_FASTEST);
	printf("fog: on\n");
    }
    else {
       glDisable(GL_FOG);
       printf("fog: off\n");
    }
    if (lighting) {
	static GLfloat ambient[4] = {1, 0.5, 0.5, 0};

	glEnable(GL_NORMALIZE);
	glNormal3f(1.0, 1.0, 1.0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	printf("lighting: on\n");
    }
    else {
       glDisable( GL_LIGHTING );
       printf("lighting: off\n");
     }
    if (shading) {
       glShadeModel(GL_SMOOTH);
       printf("shading: smooth\n");
    }
    else {
       glShadeModel(GL_FLAT);
       printf("shading: flat\n");
    }
    if (texturing) {
	static GLfloat modulate[1] = {GL_DECAL};
	static GLfloat clamp[1] = {GL_CLAMP};
	static GLfloat linear[1] = {GL_LINEAR};

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE,
		     (GLvoid *)texture);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, modulate);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear);
	glEnable(GL_TEXTURE_2D);
	printf("texturing: on\n");
    }
    else {
       glDisable( GL_TEXTURE_2D );
       printf("texturing: off\n");
     }

    Viewport(0, 0); Points();
    Viewport(0, 1); Lines();
    Viewport(0, 2); Triangles();
    Viewport(0, 3); Rects();

    glFlush();

    if (doubleBuffer) {
	tkSwapBuffers();
    }
}

static GLenum Args(int argc, char **argv)
{
    GLint i;

    rgb = GL_TRUE;
    doubleBuffer = GL_FALSE;
    directRender = GL_TRUE;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-ci") == 0) {
	    rgb = GL_FALSE;
	} else if (strcmp(argv[i], "-rgb") == 0) {
	    rgb = GL_TRUE;
	} else if (strcmp(argv[i], "-sb") == 0) {
	    doubleBuffer = GL_FALSE;
	} else if (strcmp(argv[i], "-db") == 0) {
	    doubleBuffer = GL_TRUE;
	} else if (strcmp(argv[i], "-dr") == 0) {
	    directRender = GL_TRUE;
	} else if (strcmp(argv[i], "-ir") == 0) {
	    directRender = GL_FALSE;
	} else {
	    printf("%s (Bad option).\n", argv[i]);
	    return GL_FALSE;
	}
    }
    return GL_TRUE;
}

void main(int argc, char **argv)
{

    if (Args(argc, argv) == GL_FALSE) {
	tkQuit();
    }

    windW = 600;
    windH = 300;
    tkInitPosition(0, 0, windW, windH);

    windType = /*TK_ALPHA |*/ TK_DEPTH;
    windType |= (rgb) ? TK_RGB : TK_INDEX;
    windType |= (doubleBuffer) ? TK_DOUBLE : TK_SINGLE;
    windType |= (directRender) ? TK_DIRECT : TK_INDIRECT;
    tkInitDisplayMode(windType);

    if (tkInitWindow("Speed Test") == GL_FALSE) {
	tkQuit();
    }

    tkExposeFunc(SetWindSize);
    tkReshapeFunc(SetWindSize);
    tkKeyDownFunc(Key);
    tkDisplayFunc(Draw);
    tkExec();
}
