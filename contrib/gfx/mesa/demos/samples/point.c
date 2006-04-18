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


#define CI_RED TK_RED
#define CI_ANTI_ALIAS_GREEN 16
#define CI_ANTI_ALIAS_YELLOW 32
#define CI_ANTI_ALIAS_RED 48


GLenum rgb, doubleBuffer, windType;
GLint windW, windH;

GLenum mode;
GLint size;
float point[3] = {
    1.0, 1.0, 0.0
};


static void Init(void)
{
    GLint i;

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

    if (!rgb) {
	for (i = 0; i < 16; i++) {
	    tkSetOneColor(i+CI_ANTI_ALIAS_RED, i/15.0, 0.0, 0.0);
	    tkSetOneColor(i+CI_ANTI_ALIAS_YELLOW, i/15.0, i/15.0, 0.0);
	    tkSetOneColor(i+CI_ANTI_ALIAS_GREEN, 0.0, i/15.0, 0.0);
	}
    }

    mode = GL_FALSE;
    size = 1;
}

static void Reshape(int width, int height)
{

    windW = (GLint)width;
    windH = (GLint)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-windW/2, windW/2, -windH/2, windH/2);
    glMatrixMode(GL_MODELVIEW);
}

static GLenum Key(int key, GLenum mask)
{

    switch (key) {
      case TK_ESCAPE:
	tkQuit();
      case TK_1:
	mode = !mode;
	break;
      case TK_W:
	size++;
	break;
      case TK_w:
	size--;
	if (size < 1) {
	    size = 1;
	}
	break;
      case TK_LEFT:
	point[0] -= 0.25;
	break;
      case TK_RIGHT:
	point[0] += 0.25;
	break;
      case TK_UP:
	point[1] += 0.25;
	break;
      case TK_DOWN:
	point[1] -= 0.25;
	break;
      default:
	return GL_FALSE;
    }
    return GL_TRUE;
}

static void Draw(void)
{

    glClear(GL_COLOR_BUFFER_BIT);

    TK_SETCOLOR(windType, TK_YELLOW);
    glBegin(GL_LINE_STRIP);
	glVertex2f(-windW/2, 0);
	glVertex2f(windW/2, 0);
    glEnd();
    glBegin(GL_LINE_STRIP);
	glVertex2f(0, -windH/2);
	glVertex2f(0, windH/2);
    glEnd();

    if (mode) {
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
    } else {
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SMOOTH);
    }

    glPointSize(size);
    if (mode) {
	(rgb) ? glColor3f(1.0, 0.0, 0.0) : glIndexf(CI_ANTI_ALIAS_RED);
    } else {
	(rgb) ? glColor3f(1.0, 0.0, 0.0) : glIndexf(CI_RED);
    }
    glBegin(GL_POINTS);
	glVertex3fv(point);
    glEnd();

    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);

    glPointSize(1);
    TK_SETCOLOR(windType, TK_GREEN);
    glBegin(GL_POINTS);
	glVertex3fv(point);
    glEnd();

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

    windW = 300;
    windH = 300;
    tkInitPosition(0, 0, windW, windH);

    windType = (rgb) ? TK_RGB : TK_INDEX;
    windType |= (doubleBuffer) ? TK_DOUBLE : TK_SINGLE;
    tkInitDisplayMode(windType);

    if (tkInitWindow("Point Test") == GL_FALSE) {
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
