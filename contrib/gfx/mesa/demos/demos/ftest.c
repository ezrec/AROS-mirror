/* ftest.c */


/*
 * This program calls fdraw in fdraw.f to exercise the Fortran
 * interface to Mesa.
 *
 * To make this demo work you may have to change the call to fdraw_()
 * below to just fdraw().
 *
 */


#include <stdio.h>
#include <string.h>
#include "gltk.h"


extern void fdraw_( int *width, int *height );


static int Width = 300;
static int Height = 300;




static void Reshape(int width, int height)
{
   Width = width;
   Height = height;
}


static void Draw(void)
{
   fdraw_( &Width, &Height );
}


void cmain_(int argc, char **argv)
{
    GLenum type;

    tkInitPosition(0, 0, Width, Height );

    type = TK_RGB | TK_SINGLE | TK_INDIRECT;
    tkInitDisplayMode(type);

    if (tkInitWindow("Fortran Test") == GL_FALSE) {
	tkQuit();
    }

    tkExposeFunc(Reshape);
    tkReshapeFunc(Reshape);
    tkDisplayFunc(Draw);
    tkExec();
}
