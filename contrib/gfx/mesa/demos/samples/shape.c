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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gltk.h"


#define OPENGL_WIDTH 24
#define OPENGL_HEIGHT 13


GLenum rgb, doubleBuffer, windType;
GLint objectIndex = 0;
GLuint bases[20];
float angleX = 0.0, angleY = 0.0, angleZ = 0.0;
float scaleX = 1.0, scaleY = 1.0, scaleZ = 1.0;
float shiftX = 0.0, shiftY = 0.0, shiftZ = 0.0;


static void Init(void)
{

    bases[0] = glGenLists(1);
    glNewList(bases[0], GL_COMPILE);
    tkWireSphere(bases[0], 5.0);
    glEndList();

    bases[1] = glGenLists(1);
    glNewList(bases[1], GL_COMPILE);
    tkSolidSphere(bases[1], 5.0);
    glEndList();

    bases[2] = glGenLists(1);
    glNewList(bases[2], GL_COMPILE);
    tkWireCube(bases[2], 5.0);
    glEndList();

    bases[3] = glGenLists(1);
    glNewList(bases[3], GL_COMPILE);
    tkSolidCube(bases[3], 5.0);
    glEndList();

    bases[4] = glGenLists(1);
    glNewList(bases[4], GL_COMPILE);
    tkWireBox(bases[4], 4.0, 5.0, 6.0);
    glEndList();

    bases[5] = glGenLists(1);
    glNewList(bases[5], GL_COMPILE);
    tkSolidBox(bases[5], 4.0, 5.0, 6.0);
    glEndList();

    bases[6] = glGenLists(1);
    glNewList(bases[6], GL_COMPILE);
    tkWireTorus(bases[6], 5.0, 5.0);
    glEndList();

    bases[7] = glGenLists(1);
    glNewList(bases[7], GL_COMPILE);
    tkSolidTorus(bases[7], 5.0, 5.0);
    glEndList();

    bases[8] = glGenLists(1);
    glNewList(bases[8], GL_COMPILE);
    tkWireCylinder(bases[8], 5.0, 5.0);
    glEndList();

    bases[9] = glGenLists(1);
    glNewList(bases[9], GL_COMPILE);
    tkSolidCylinder(bases[9], 5.0, 5.0);
    glEndList();

    bases[10] = glGenLists(1);
    glNewList(bases[10], GL_COMPILE);
    tkWireCone(bases[10], 5.0, 5.0);
    glEndList();

    bases[11] = glGenLists(1);
    glNewList(bases[11], GL_COMPILE);
    tkSolidCone(bases[11], 5.0, 5.0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearIndex(0.0);
}

static void Reshape(int width, int height)
{

    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-400.0, 400.0, -200.0, 200.0, -400.0, 400.0);
    glMatrixMode(GL_MODELVIEW);
}

static GLenum Key(int key, GLenum mask)
{

    switch (key) {
      case TK_ESCAPE:
        tkQuit();

      case TK_SPACE:
	objectIndex++;
	if (objectIndex > 11) {
	    objectIndex = 0;
	}
	break;

      case TK_LEFT:
	shiftX -= 20.0;
	break;
      case TK_RIGHT:
	shiftX += 20.0;
	break;
      case TK_UP:
	shiftY += 20.0;
	break;
      case TK_DOWN:
	shiftY -= 20.0;
	break;
      case TK_n:
	shiftZ += 20.0;
	break;
      case TK_m:
	shiftZ -= 20.0;
	break;

      case TK_q:
	scaleX -= 0.1;
	if (scaleX < 0.1) {
	    scaleX = 0.1;
	}
	break;
      case TK_w:
	scaleX += 0.1;
	break;
      case TK_a:
	scaleY -= 0.1;
	if (scaleY < 0.1) {
	    scaleY = 0.1;
	}
	break;
      case TK_s:
	scaleY += 0.1;
	break;
      case TK_z:
	scaleZ -= 0.1;
	if (scaleZ < 0.1) {
	    scaleZ = 0.1;
	}
	break;
      case TK_x:
	scaleZ += 0.1;
	break;

      case TK_e:
	angleX -= 5.0;
	if (angleX < 0.0) {
	    angleX = 360.0 + angleX;
	}
	break;
      case TK_r:
	angleX += 5.0;
	if (angleX > 360.0) {
	    angleX = angleX - 360.0;
	}
	break;
      case TK_d:
	angleY -= 5.0;
	if (angleY < 0.0) {
	    angleY = 360.0 + angleY;
	}
	break;
      case TK_f:
	angleY += 5.0;
	if (angleY > 360.0) {
	    angleY = angleY - 360.0;
	}
	break;
      case TK_c:
	angleZ -= 5.0;
	if (angleZ < 0.0) {
	    angleZ = 360.0 + angleZ;
	}
	break;
      case TK_v:
	angleZ += 5.0;
	if (angleZ > 360.0) {
	    angleZ = angleZ - 360.0;
	}
	break;
      default:
	return GL_FALSE;
    }
    return GL_TRUE;
}

static void Draw(void)
{

    glClear(GL_COLOR_BUFFER_BIT);

    TK_SETCOLOR(windType, TK_WHITE);

    glPushMatrix();

    glTranslatef(shiftX, shiftY, shiftZ);
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    glRotatef(angleZ, 0.0, 0.0, 1.0);
    glScalef(scaleX, scaleY, scaleZ);

    glCallList(bases[objectIndex]);
    glPopMatrix();

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

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-ci") == 0) {
	    rgb = GL_FALSE;
	} else if (strcmp(argv[i], "-rgb") == 0) {
	    rgb = GL_TRUE;
	} else if (strcmp(argv[i], "-sb") == 0) {
	    doubleBuffer = GL_FALSE;
	} else if (strcmp(argv[i], "-db") == 0) {
	    doubleBuffer = GL_TRUE;
	} else {
	    printf("%s (Bad option).\n", argv[i]);
	    return GL_FALSE;
	}
    }
    return GL_TRUE;
}

int main(int argc, char **argv)
{

    if (Args(argc, argv) == GL_FALSE) {
	tkQuit();
    }

    tkInitPosition(0, 0, 400, 400);

    windType = (rgb) ? TK_RGB : TK_INDEX;
    windType |= (doubleBuffer) ? TK_DOUBLE : TK_SINGLE;
    tkInitDisplayMode(windType);

    if (tkInitWindow("Font Test") == GL_FALSE) {
	tkQuit();
    }

    Init();

    tkExposeFunc(Reshape);
    tkReshapeFunc(Reshape);
    tkKeyDownFunc(Key);
    tkDisplayFunc(Draw);
    tkExec();
	return 0;
}
