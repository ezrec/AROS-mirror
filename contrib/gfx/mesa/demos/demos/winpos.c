/* winpos.c */

/*
 * Example of how to use the GL_MESA_window_pos extension.
 *
 * Brian Paul
 */


#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include "gltk.h"

#define IMAGE_FILE "/images/girl.rgb"

#ifndef M_PI
#  define M_PI 3.14159265
#endif


static TK_RGBImageRec *image;



static void draw( void )
{
   GLfloat angle;
   char *extensions;

   extensions = (char *) glGetString( GL_EXTENSIONS );
   if (strstr( extensions, "GL_MESA_window_pos")==NULL) {
      printf("Sorry, GL_MESA_window_pos extension not available.\n");
      return;
   }

   glClear( GL_COLOR_BUFFER_BIT );

   for (angle = -45.0; angle <= 135.0; angle += 10.0) {
      GLfloat x = 50.0 + 200.0 * cos( angle * M_PI / 180.0 );
      GLfloat y = 50.0 + 200.0 * sin( angle * M_PI / 180.0 );

      /* Don't need to worry about the modelview or projection matrices!!! */
#ifdef GL_MESA_window_pos
      glWindowPos2fMESA( x, y );
#endif
      glDrawPixels( image->sizeX, image->sizeY, GL_RGB,
                    GL_UNSIGNED_BYTE, image->data );
   }
}




static GLenum key(int k, GLenum mask)
{
   switch (k) {
      case TK_ESCAPE:
	 tkQuit();
   }
   return GL_FALSE;
}



/* new window size or exposure */
static void reshape( int width, int height )
{
   glViewport(0, 0, (GLint)width, (GLint)height);
}


static void init( void )
{
   image = tkRGBImageLoad( IMAGE_FILE );

   if (!image) {
      printf("Error: couldn't load image file: %s\n", IMAGE_FILE );
      exit(1);
   }
}


int main( int argc, char *argv[] )
{
   tkInitPosition(0, 0, 500, 500);
   tkInitDisplayMode( TK_RGB | TK_DIRECT );

   if (tkInitWindow("winpos") == GL_FALSE) {
      tkQuit();
   }

   init();

   tkExposeFunc( reshape );
   tkReshapeFunc( reshape );
   tkKeyDownFunc( key );
   tkDisplayFunc( draw );
   tkExec();
   return 0;
}
