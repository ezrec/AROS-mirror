/* test0.c */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "glaux.h"




static void Init( void )
{
   /* one-time init (clearColor, set palette, etc) */

   glClearIndex( 0.0 );
   glShadeModel( GL_FLAT );
}


static void Reshape( int width, int height )
{
   glViewport(0, 0, (GLint)width, (GLint)height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
   glMatrixMode(GL_MODELVIEW);
}


static void key_up()
{
   printf("AUX_UP\n");
}


static void key_down()
{
   printf("AUX_DOWN\n");
}


static void key_esc()
{
   auxQuit();
}


static void display( void )
{
   /* clear viewport */
   glClear( GL_COLOR_BUFFER_BIT );

   /* draw stuff */
   glIndexi( 1 );
   glBegin( GL_LINES );
   glVertex3f( 0.0, 0.0, 0.0 );
   glVertex3f( 1.0, 0.0, 0.0 );
   glEnd();

   glBegin( GL_LINES );
   glVertex3f( 0.0, 0.0, 0.0 );
   glVertex3f( 0.0, 0.5, 0.0 );
   glEnd();


   /* flush / swap buffers */
   glFlush();
   auxSwapBuffers();
}



int main( int argc, char **argv )
{
   auxInitDisplayMode( AUX_INDEX );

   auxInitPosition( 50, 50, 400, 300 );

   if (auxInitWindow("test0") == GL_FALSE) {
      auxQuit();
   }

   Init();

   auxExposeFunc(Reshape);
   auxReshapeFunc(Reshape);
   auxKeyFunc( AUX_UP, key_up );
   auxKeyFunc( AUX_DOWN, key_down );

   auxMainLoop( display );

   return 0;
}
