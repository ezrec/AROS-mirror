/* glutskel.c */

/*
 * A skeleton/template GLUT program
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>



static void Idle( void )
{

}


static void Display( void )
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   /* draw stuff here */

   glutSwapBuffers();
}


static void Reshape( int width, int height )
{
   glViewport( 0, 0, width, height );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 25.0 );
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, -15.0 );
}


static void Key( unsigned char key, int x, int y )
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
   glutPostRedisplay();
}


static void SpecialKey( int key, int x, int y )
{
   switch (key) {
      case GLUT_KEY_UP:
         break;
      case GLUT_KEY_DOWN:
         break;
   }
   glutPostRedisplay();
}


static void Init( void )
{
   /* setup lighting, etc */
}


int main( int argc, char *argv[] )
{

   glutInit( &argc, argv );
   glutInitWindowSize( 400, 400 );

   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

   glutCreateWindow( "my window" );

   Init();

   glutReshapeFunc( Reshape );
   glutKeyboardFunc( Key );
   glutSpecialFunc( SpecialKey );
   glutDisplayFunc( Display );
   glutIdleFunc( Idle );

   glutMainLoop();
}
