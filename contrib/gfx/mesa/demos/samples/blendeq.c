/*
** blendeq.c - Demonstrates the use of the blend_minmax, blend_subtract,
**    and blend_logic_op extensions using glBlendEquationEXT.
**
**    Over a two-color backround, draw rectangles using twelve blend
**    options.  The values are read back as UNSIGNED_BYTE and printed
**    in hex over each value.  These values are useful for logic
**    op comparisons when channels are 8 bits deep.
*/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "gltk.h"


GLenum doubleBuffer, directRender;
static int dithering = 0;
static int doPrint = 1;
static int deltaY;
GLint windW, windH;
GLuint bitmapBase;

static void Init(void)
{
    bitmapBase = glGenLists(256);
    if (tkCreateBitmapFont(bitmapBase) == GL_FALSE) {
	tkQuit();
    }

    glDisable(GL_DITHER);
    glShadeModel(GL_FLAT);
}

static void Reshape(int width, int height)
{

    windW = (GLint)width;
    windH = (GLint)height;

    glViewport(0, 0, (GLint)width, (GLint)height);
    deltaY = windH /16;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windW, 0, windH);
    glMatrixMode(GL_MODELVIEW);
}

static GLenum Key(int key, GLenum mask)
{

    switch (key) {
      case TK_ESCAPE:
	tkQuit();
      case TK_d:
	dithering = !dithering;
	break;
      default:
	return GL_FALSE;
    }
    return GL_TRUE;
}

static void PrintColorStrings( void )
{
    GLubyte ubbuf[3];
    int i, xleft, xright;
    char colorString[18];

    xleft = 5 + windW/4;
    xright = 5 + windW/2;

    for (i = windH - deltaY + 4; i > 0; i-=deltaY) {
        glReadPixels(xleft, i+10, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, ubbuf);
        sprintf(colorString, "(0x%x, 0x%x, 0x%x)",
                ubbuf[0], ubbuf[1], ubbuf[2]);
        glRasterPos2f(xleft, i);
	tkDrawStr(bitmapBase, colorString);
        glReadPixels(xright, i+10, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, ubbuf);
        sprintf(colorString, "(0x%x, 0x%x, 0x%x)",
                ubbuf[0], ubbuf[1], ubbuf[2]);
        glRasterPos2f(xright, i);
	tkDrawStr(bitmapBase, colorString);
    }
}

static void Draw(void)
{
    int stringOffset = 5, stringx = 8;
    int x1, x2, xleft, xright;
    int i;

    (dithering) ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
    glDisable(GL_BLEND);

    glClearColor(0.5, 0.6, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw background */
    glColor3f(0.1, 0.1, 1.0);
    glRectf(0.0, 0.0, windW/2, windH);

    /* Draw labels */
    glColor3f(0.8, 0.8, 0.0);
    i = windH - deltaY + stringOffset;
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "SOURCE");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "DEST");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "min");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "max");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "subtract");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "reverse_subtract");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "clear");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "set");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "copy");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "noop");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "and");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "invert");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "or");
    glRasterPos2f(stringx, i); i -= deltaY;
    tkDrawStr(bitmapBase, "xor");


    i = windH - deltaY;
    x1 = windW/4;
    x2 = 3 * windW/4;
    xleft = 5 + windW/4;
    xright = 5 + windW/2;

    /* Draw foreground color for comparison */
    glColor3f(0.9, 0.2, 0.8);
    glRectf(x1, i, x2, i+deltaY);

    /* Leave one rectangle of background color */

    /* Begin test cases */
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    i -= 2*deltaY;
    glBlendEquationEXT(GL_MIN_EXT);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_MAX_EXT);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_FUNC_SUBTRACT_EXT);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);
    glRectf(x1, i, x2, i+deltaY);

    glBlendFunc(GL_ONE, GL_ZERO);
    i -= deltaY;
    glBlendEquationEXT(GL_LOGIC_OP);
    glLogicOp(GL_CLEAR);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_LOGIC_OP);
    glLogicOp(GL_SET);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_LOGIC_OP);
    glLogicOp(GL_COPY);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_LOGIC_OP);
    glLogicOp(GL_NOOP);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_LOGIC_OP);
    glLogicOp(GL_AND);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_LOGIC_OP);
    glLogicOp(GL_INVERT);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_LOGIC_OP);
    glLogicOp(GL_OR);
    glRectf(x1, i, x2, i+deltaY);

    i -= deltaY;
    glBlendEquationEXT(GL_LOGIC_OP);
    glLogicOp(GL_XOR);
    glRectf(x1, i, x2, i+deltaY);
    glRectf(x1, i+10, x2, i+5);

  if (doPrint) {
      glDisable(GL_BLEND);
      glColor3f(1.0, 1.0, 1.0);
      PrintColorStrings();
  }
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
    char *s;
    char *extName1 = "GL_EXT_blend_logic_op";
    char *extName2 = "GL_EXT_blend_minmax";
    char *extName3 = "GL_EXT_blend_subtract";

    if (Args(argc, argv) == GL_FALSE) {
	tkQuit();
    }

    tkInitPosition(0, 0, 800, 400);

    type = TK_RGB;
    type |= (doubleBuffer) ? TK_DOUBLE : TK_SINGLE;
    type |= (directRender) ? TK_DIRECT : TK_INDIRECT;
    tkInitDisplayMode(type);

    if (tkInitWindow("Blend Equation") == GL_FALSE) {
	tkQuit();
    }

    /* Make sure blend_logic_op extension is there. */
    s = (char *) glGetString(GL_EXTENSIONS);
    if (!s)
	tkQuit();
    if (strstr(s,extName1) == 0) {
	printf("Blend_logic_op extension is not present.\n");
	tkQuit();
    }
    if (strstr(s,extName2) == 0) {
	printf("Blend_minmax extension is not present.\n");
	tkQuit();
    }
    if (strstr(s,extName3) == 0) {
	printf("Blend_subtract extension is not present.\n");
	tkQuit();
    }

    Init();

    tkExposeFunc(Reshape);
    tkReshapeFunc(Reshape);
    tkKeyDownFunc(Key);
    tkDisplayFunc(Draw);
    tkExec();
}
