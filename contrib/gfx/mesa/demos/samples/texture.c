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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include "gltk.h"


GLenum doubleBuffer, directRender;

char *texFileName = 0;
TK_RGBImageRec *image;

float *minFilter, *magFilter, *sWrapMode, *tWrapMode;
float decal[] = {GL_DECAL};
float modulate[] = {GL_MODULATE};
float repeat[] = {GL_REPEAT};
float clamp[] = {GL_CLAMP};
float nr[] = {GL_NEAREST};
float ln[] = {GL_LINEAR};
float nr_mipmap_nr[] = {GL_NEAREST_MIPMAP_NEAREST};
float nr_mipmap_ln[] = {GL_NEAREST_MIPMAP_LINEAR};
float ln_mipmap_nr[] = {GL_LINEAR_MIPMAP_NEAREST};
float ln_mipmap_ln[] = {GL_LINEAR_MIPMAP_LINEAR};
GLint sphereMap[] = {GL_SPHERE_MAP};

GLenum doSphere = GL_FALSE;
float xRotation = 0.0, yRotation = 0.0, zTranslate = -3.125;

GLint cube;
float c[6][4][3] = {
    {
	{
	    1.0, 1.0, -1.0
	},
	{
	    -1.0, 1.0, -1.0
	},
	{
	    -1.0, -1.0, -1.0
	},
	{
	    1.0, -1.0, -1.0
	}
    },
    {
	{
	    1.0, 1.0, 1.0
	},
	{
	    1.0, 1.0, -1.0
	},
	{
	    1.0, -1.0, -1.0
	},
	{
	    1.0, -1.0, 1.0
	}
    },
    {
	{
	    -1.0, 1.0, 1.0
	},
	{
	    1.0, 1.0, 1.0
	},
	{
	    1.0, -1.0, 1.0
	},
	{
	    -1.0, -1.0, 1.0
	}
    },
    {
	{
	    -1.0, 1.0, -1.0
	},
	{
	    -1.0, 1.0, 1.0
	},
	{
	    -1.0, -1.0, 1.0
	},
	{
	    -1.0, -1.0, -1.0
	}
    },
    {
	{
	    -1.0, 1.0, 1.0
	},
	{
	    -1.0, 1.0, -1.0
	},
	{
	    1.0, 1.0, -1.0
	},
	{
	    1.0, 1.0, 1.0
	}
    },
    {
	{
	    -1.0, -1.0, -1.0
	},
	{
	    -1.0, -1.0, 1.0
	},
	{
	    1.0, -1.0, 1.0
	},
	{
	    1.0, -1.0, -1.0
	}
    }
};
static float n[6][3] = {
    {
	0.0, 0.0, -1.0
    },
    {
	1.0, 0.0, 0.0
    },
    {
	0.0, 0.0, 1.0
    },
    {
	-1.0, 0.0, 0.0
    },
    {
	0.0, 1.0, 0.0
    },
    {
	0.0, -1.0, 0.0
    }
};
static float t[6][4][2] = {
    {
	{
	    1.1,  1.1
	},
	{
	    -0.1, 1.1
	},
	{
	    -0.1, -0.1
	},
	{
	    1.1,  -0.1
	}
    },
    {
	{
	    1.1,  1.1
	},
	{
	    -0.1, 1.1
	},
	{
	    -0.1, -0.1
	},
	{
	    1.1,  -0.1
	}
    },
    {
	{
	    -0.1,  1.1
	},
	{
	    1.1, 1.1
	},
	{
	    1.1, -0.1
	},
	{
	    -0.1,  -0.1
	}
    },
    {
	{
	    1.1,  1.1
	},
	{
	    -0.1, 1.1
	},
	{
	    -0.1, -0.1
	},
	{
	    1.1,  -0.1
	}
    },
    {
	{
	    1.1,  1.1
	},
	{
	    -0.1, 1.1
	},
	{
	    -0.1, -0.1
	},
	{
	    1.1,  -0.1
	}
    },
    {
	{
	    1.1,  1.1
	},
	{
	    -0.1, 1.1
	},
	{
	    -0.1, -0.1
	},
	{
	    1.1,  -0.1
	}
    },
};


static void BuildCube(void)
{
    GLint i;

    glNewList(cube, GL_COMPILE);
    for (i = 0; i < 6; i++) {
	glBegin(GL_POLYGON);
	    glNormal3fv(n[i]); glTexCoord2fv(t[i][0]); glVertex3fv(c[i][0]);
	    glNormal3fv(n[i]); glTexCoord2fv(t[i][1]); glVertex3fv(c[i][1]);
	    glNormal3fv(n[i]); glTexCoord2fv(t[i][2]); glVertex3fv(c[i][2]);
	    glNormal3fv(n[i]); glTexCoord2fv(t[i][3]); glVertex3fv(c[i][3]);
	glEnd();
    }
    glEndList();
}

static void BuildLists(void)
{

    cube = glGenLists(1);
    BuildCube();
}

static void Init(void)
{

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->sizeX, image->sizeY,
		      GL_RGB, GL_UNSIGNED_BYTE, image->data);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, decal);
    glEnable(GL_TEXTURE_2D);

    glFrontFace(GL_CCW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    BuildLists();

    glClearColor(0.0, 0.0, 0.0, 0.0);

    magFilter = nr;
    minFilter = nr;
    sWrapMode = repeat;
    tWrapMode = repeat;
}

static void Reshape(int width, int height)
{

    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(145.0, 1.0, 0.01, 1000);
    glMatrixMode(GL_MODELVIEW);
}

static GLenum Key(int key, GLenum mask)
{

    switch (key) {
      case TK_ESCAPE:
	tkQuit();

      case TK_LEFT:
	yRotation -= 0.5;
	break;
      case TK_RIGHT:
	yRotation += 0.5;
	break;
      case TK_UP:
	xRotation -= 0.5;
	break;
      case TK_DOWN:
	xRotation += 0.5;
	break;
      case TK_T:
	zTranslate += 0.25;
	break;
      case TK_t:
	zTranslate -= 0.25;
	break;

      case TK_s:
	doSphere = !doSphere;
	if (doSphere) {
	    glTexGeniv(GL_S, GL_TEXTURE_GEN_MODE, sphereMap);
	    glTexGeniv(GL_T, GL_TEXTURE_GEN_MODE, sphereMap);
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
	} else {
	    glDisable(GL_TEXTURE_GEN_S);
	    glDisable(GL_TEXTURE_GEN_T);
	}
	break;

      case TK_0:
	magFilter = nr;
	break;
      case TK_1:
	magFilter = ln;
	break;
      case TK_2:
	minFilter = nr;
	break;
      case TK_3:
	minFilter = ln;
	break;
      case TK_4:
	minFilter = nr_mipmap_nr;
	break;
      case TK_5:
	minFilter = nr_mipmap_ln;
	break;
      case TK_6:
	minFilter = ln_mipmap_nr;
	break;
      case TK_7:
	minFilter = ln_mipmap_ln;
	break;

      default:
	return GL_FALSE;
    }
    return GL_TRUE;
}

static void Draw(void)
{

    glClear(GL_COLOR_BUFFER_BIT);

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapMode);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapMode);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

    glPushMatrix();

    glTranslatef(0.0, 0.0, zTranslate);
    glRotatef(xRotation, 1, 0, 0);
    glRotatef(yRotation, 0, 1, 0);
    glCallList(cube);

    glPopMatrix();

    glFlush();

    if (doubleBuffer) {
	tkSwapBuffers();
    }
}

static GLenum Args(int argc, char **argv)
{
    GLint i;

    doubleBuffer = GL_FALSE;
    directRender = GL_TRUE;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-sb") == 0) {
	    doubleBuffer = GL_FALSE;
	} else if (strcmp(argv[i], "-db") == 0) {
	    doubleBuffer = GL_TRUE;
	} else if (strcmp(argv[i], "-dr") == 0) {
	    directRender = GL_TRUE;
	} else if (strcmp(argv[i], "-ir") == 0) {
	    directRender = GL_FALSE;
	} else if (strcmp(argv[i], "-f") == 0) {
	    if (i+1 >= argc || argv[i+1][0] == '-') {
		printf("-f (No file name).\n");
		return GL_FALSE;
	    } else {
		texFileName = argv[++i];
	    }
	} else {
	    printf("%s (Bad option).\n", argv[i]);
	    return GL_FALSE;
	}
    }
    return GL_TRUE;
}

void main(int argc, char **argv)
{
    GLenum type;

    if (Args(argc, argv) == GL_FALSE) {
	tkQuit();
    }

    if (texFileName == 0) {
	printf("No image file.\n");
	tkQuit();
    }

    image = tkRGBImageLoad(texFileName);

    tkInitPosition(0, 0, 300, 300);

    type = TK_RGB;
    type |= (doubleBuffer) ? TK_DOUBLE : TK_SINGLE;
    type |= (directRender) ? TK_DIRECT : TK_INDIRECT;
    tkInitDisplayMode(type);

    if (tkInitWindow("Texture Test") == GL_FALSE) {
	tkQuit();
    }

    Init();

    tkExposeFunc(Reshape);
    tkReshapeFunc(Reshape);
    tkKeyDownFunc(Key);
    tkDisplayFunc(Draw);
    tkExec();
}
