/* trispd.c */

/*
 * Simple GLUT program to measure triangle strip rendering speed.
 * Brian Paul  February 15, 1997
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <sys/times.h>
#include <sys/param.h>


/*
 * Return system time in seconds.
 * NOTE:  this implementation may not be very portable!
 */
GLdouble GetTime( void )
{
   static GLdouble prev_time = 0.0;
   static GLdouble time;
   struct tms tm;
   clock_t clk;

   clk = times(&tm);

#ifdef CLK_TCK
   time = (double)clk / (double)CLK_TCK;
#else
   time = (double)clk / (double)HZ;
#endif

   if (time>prev_time) {
      prev_time = time;
      return time;
   }
   else {
      return prev_time;
   }
}


static float MinPeriod = 2.0;   /* 2 seconds */
static float Width = 400.0;
static float Height = 400.0;
static int Loops = 1;
static int Size = 50;



static void Idle( void )
{
   glutPostRedisplay();
}


static void Display( void )
{
   float x, y;
   float xStep;
   float yStep;
   double t0, t1;
   double triRate;
   double pixelRate;
   int triCount;
   int i;
   float red[3] = { 1.0, 0.0, 0.0 };
   float blue[3] = { 0.0, 0.0, 1.0 };

   xStep = yStep = sqrt( 2.0 * Size );

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   triCount = 0;
   t0 = GetTime();
   for (i=0; i<Loops; i++) {
      for (y=1.0; y<Height-yStep; y+=yStep) {
         glBegin(GL_TRIANGLE_STRIP);
         for (x=1.0; x<Width; x+=xStep) {
            glColor3fv(red);
            glVertex2f(x, y);
            glColor3fv(blue);
            glVertex2f(x, y+yStep);
            triCount += 2;
         }
         glEnd();
         triCount -= 2;
      }
   }
   t1 = GetTime();

   if (t1-t0 < MinPeriod) {
      /* Next time draw more triangles to get longer elapsed time */
      Loops *= 2;
      return;
   }

   triRate = triCount / (t1-t0);
   pixelRate = triRate * Size;
   printf("Rate: %d tri in %gs = %g tri/s  %d pixels/s\n",
          triCount, t1-t0, triRate, (int)pixelRate);

   glutSwapBuffers();
}


static void Reshape( int width, int height )
{
   Width = width;
   Height = height;
   glViewport( 0, 0, width, height );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
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


static void Init( int argc, char *argv[] )
{
   GLint shade;
   GLint rBits, gBits, bBits;

   int i;
   for (i=1; i<argc; i++) {
      if (strcmp(argv[i],"-dither")==0)
         glDisable(GL_DITHER);
      else if (strcmp(argv[i],"+dither")==0)
         glEnable(GL_DITHER);
      else if (strcmp(argv[i],"+smooth")==0)
         glShadeModel(GL_SMOOTH);
      else if (strcmp(argv[i],"+flat")==0)
         glShadeModel(GL_FLAT);
      else if (strcmp(argv[i],"+depth")==0)
         glEnable(GL_DEPTH_TEST);
      else if (strcmp(argv[i],"-depth")==0)
         glDisable(GL_DEPTH_TEST);
      else if (strcmp(argv[i],"-size")==0) {
         Size = atoi(argv[i+1]);
         i++;
      }
      else
         printf("Unknown option: %s\n", argv[i]);
   }

   glGetIntegerv(GL_SHADE_MODEL, &shade);

   printf("Dither: %s\n", glIsEnabled(GL_DITHER) ? "on" : "off");
   printf("ShadeModel: %s\n", (shade==GL_FLAT) ? "flat" : "smooth");
   printf("DepthTest: %s\n", glIsEnabled(GL_DEPTH_TEST) ? "on" : "off");
   printf("Size: %d pixels\n", Size);

   glGetIntegerv(GL_RED_BITS, &rBits);
   glGetIntegerv(GL_GREEN_BITS, &gBits);
   glGetIntegerv(GL_BLUE_BITS, &bBits);
   printf("RedBits: %d  GreenBits: %d  BlueBits: %d\n", rBits, gBits, bBits);
}


static void Help( const char *program )
{
   printf("%s options:\n", program);
   printf("  +/-dither      enable/disable dithering\n");
   printf("  +/-depth       enable/disable depth test\n");
   printf("  +flat          flat shading\n");
   printf("  +smooth        smooth shading\n");
   printf("  -size pixels   specify pixels/triangle\n");
}


int main( int argc, char *argv[] )
{
   printf("For options:  %s -help\n", argv[0]);
   glutInit( &argc, argv );
   glutInitWindowSize( (int) Width, (int) Height );
   glutInitWindowPosition( 0, 0 );

   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

   glutCreateWindow( argv[0] );

   if (argc==2 && strcmp(argv[1],"-help")==0) {
      Help(argv[0]);
      return 0;
   }

   Init( argc, argv );

   glutReshapeFunc( Reshape );
   glutKeyboardFunc( Key );
   glutDisplayFunc( Display );
   glutIdleFunc( Idle );

   glutMainLoop();
}
