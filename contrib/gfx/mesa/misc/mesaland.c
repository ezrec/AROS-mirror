/*

				Terrain Written by SkiZoWalker (MoDEL) / France
	                 Using Mesa 2.2 - 3DFX extension

       Ugly Code just to test the speed between Mesa/3DFX and OpenGL32.dll
       Coming from GLQuake .. Seems that OpenGL32.DLL Is faster !!

       Note : The code is UGLY (i only code in C since 2 weeks) , i know it 
		 					 don't Blame me , No time to
       Rewrite nice code , just wanted to test what power is :)) !

       Write me at : Skizo@Hol.Fr

       Greetings to David B. , he knows why !!

	-----------------------------------------------------------------------
					        Link with Mesa , TK , Glu , Glide
	go to Links page of http://wwwperso.hol.fr/~mgilaber/ to know where downloading
	these libs if they miss you.
	-----------------------------------------------------------------------


*/


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <gltk.h>

#ifdef FX
#include <fx/fxmesa.h>
#endif

#ifndef M_PI
#  define M_PI 3.14159265f
#endif


#define 	heightMnt	3.0f
#define	lenghtXmnt	40
#define 	lenghtYmnt	40
        
#define 	stepXmnt     (2.0f/lenghtXmnt)*2.0f
#define 	stepYmnt     (2.0f/lenghtYmnt)*2.0f

#define 	WIDTH        640
#define 	HEIGHT       480

#define 	ROLL_SPEED   0.0f//1.0
#define 	YAW_SPEED    0.5f//1.0
#define 	PITCH_SPEED  1.5f

#define 	TEX_WIDTH    256
#define 	TEX_HEIGHT   256
#define 	NUM_TEXTURES 2

GLuint texobjs[NUM_TEXTURES];
GLubyte image[NUM_TEXTURES][TEX_WIDTH][TEX_HEIGHT][4];
GLubyte pic[256][256][4];
GLubyte terrainpic[256][256][4];
GLubyte bufferpic[256][256][3];
GLubyte bufferter[256*256];
GLfloat terrain[256*256];

GLvoid drawScene(GLvoid);
float ModZMnt;
long GlobalMnt;
double GlobalSky;

void moveterrain (void);
void loadpic (void);
void drawterrain(void);
void drawsky (void);


GLvoid drawScene(GLvoid)
{
  static float roll  = 0;
  static float yaw   = 0;
  static float yaw2  = 0;
  static float pitch = 40.0;             //50

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDepthMask(GL_FALSE);
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -6.0f);
  glScalef (18.0f ,12.0f ,12.0f);
  glRotatef(roll,  0.0f, 0.0f, 1.0f);
  glRotatef(170.0f,1.0f, 0.0f, 0.0f);//175
  glTranslatef(0.0f, -0.2f,  0.0f);
  drawsky();
  glPopMatrix();

  glDepthMask(GL_TRUE);
  glPushMatrix();
  glTranslatef(0.0f,  0.0f, -7.5f);
  glScalef (8.0f ,3.5f ,12.0f);
  glRotatef(roll,  0.0f, 0.0f, 1.0f);
  glRotatef(pitch, 1.0f, 0.0f, 0.0f); // a // pour effet eau !
  glRotatef(yaw,   0.0f, 1.0f, 0.0f);
  glTranslatef(0.0f,  -0.5f,  0.0f);
  drawterrain();
  glPopMatrix();


  tkSwapBuffers();

//    SwapBuffers(ghDC);

  roll  += ROLL_SPEED;  if (roll  > 360.0f) roll  -= 360.0f;
  yaw   += YAW_SPEED;   if (yaw   > 360.0f) yaw   -= 360.0f;
  yaw2  -= YAW_SPEED;   if (yaw2	< 0.0f) 	 yaw2+= 360.0f;
}

void drawterrain (void)
{
  unsigned long h, i;
  float j, k ;

  for (h = 0, k = -1.0f; h < lenghtYmnt; k += stepYmnt, h += 2)
  {
    for (i = 0, j = -1.0f; i < lenghtXmnt; j += stepXmnt, i += 2)
    {
      glBegin(GL_QUADS);
      glColor3f(0.0f, terrain[(i+(h*256)+GlobalMnt)&65535], terrain[(i+(h*256)+GlobalMnt)&65535]+0.03f);
      glVertex3f( j, terrain[(i+(h*256)+GlobalMnt)&65535]*heightMnt, k);
      glColor3f(0.0f, terrain[(i+(h*256)+2+GlobalMnt)&65535], terrain[(i+(h*256)+2+GlobalMnt)&65535]+0.03f);
      glVertex3f( j+stepXmnt, terrain[(i+(h*256)+2+GlobalMnt)&65535]*heightMnt, k);
      glColor3f(0.0f, terrain[(i+(h*256)+2+256*2+GlobalMnt)&65535], terrain[(i+(h*256)+2+256*2+GlobalMnt)&65535]+0.03f);
      glVertex3f( j+stepXmnt, terrain[(i+(h*256)+2+256*2+GlobalMnt)&65535]*heightMnt, k+stepYmnt);
	   glColor3f(0.0f, terrain[(i+(h*256)+256*2+GlobalMnt)&65535], terrain[(i+(h*256)+256*2+GlobalMnt)&65535]+0.03f);
      glVertex3f( j, terrain[(i+(h*256)+256*2+GlobalMnt)&65535]*heightMnt, k+stepYmnt);
    }
    glEnd();
  }
}

void drawsky (void)
{
  double texwarpS[8] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f};
  double texwarpD[8] = {0.0};
  static double angle = 0.0;
  int i;

  for (i = 0; i < 8; i += 2)
  {
    texwarpD[i]  =(((texwarpS[i]*cos(angle))-(texwarpS[i+1]*sin(angle)))+0.5);
    texwarpD[i+1]=(((texwarpS[i+1]*cos(angle))+(texwarpS[i]*sin(angle)))+0.5)-GlobalSky;
  }
   
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);

  glTexCoord2f(texwarpD[0], texwarpD[1]); glVertex3f( 1.0f, -0.0f, -1.0f);
  glTexCoord2f(texwarpD[2], texwarpD[3]); glVertex3f( 1.0f,  0.0f,  1.0f);
  glTexCoord2f(texwarpD[4], texwarpD[5]); glVertex3f(-1.0f,  0.0f,  1.0f);
  glTexCoord2f(texwarpD[6], texwarpD[7]); glVertex3f(-1.0f, -0.0f, -1.0f);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  angle  += (6.3f/360.0f);  if (angle > 360.0f) angle -= 360.0f;
}

/*--------------------------------------------------------------------------*/
/*							         Mouvement du terrain									 */
/*--------------------------------------------------------------------------*/

void moveterrain (void)
{
//   if (ModZMnt<=(-0.f))
//   { ModZMnt=0.0f;
  GlobalMnt += 256;
  GlobalSky += 0.008f;
//	}
//   else ModZMnt -=0.01f;
}

/*--------------------------------------------------------------------------*/
/*						 Chargement et transformation de fichiers						 */
/*--------------------------------------------------------------------------*/

void loadpic (void)
{
  FILE *f;
  unsigned long i,j;

  if ((f = fopen("mnt.bin","r")) == NULL) {
#if !defined(__AROS__)
    MessageBox(NULL, "File Not Found : mnt.Bin", "File Not Found", MB_ICONSTOP);
#else
    printf("Couldnt find texture file");
#endif
    exit (1);
  }

  fread (bufferter , 256*256 , 1 , f);
  fclose (f);

  for (i = 0; i < (256 * 255); i++)
  {
    terrain[i]=(((GLfloat)bufferter[i]) * ((1.0f / 255.0f))) - 0.5f;
  }

  for (i = 0; i < 256; i++)
  {
    for (j = 0; j < 256; j++)
    {
      terrainpic[i][j][0] = 0; 
      terrainpic[i][j][1] = ((bufferter[i + j * 256]) << 1);  
      terrainpic[i][j][2] = ((bufferter[i + j * 256]) << 1); 
      terrainpic[i][j][3] = 255; 
    }
  }
}

/* new window size or exposure */
static void reshape( int width, int height )
{
  GLfloat  h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint)width, (GLint)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef( 0.0, 0.0, -18.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


static void init( void )
{
  GLfloat aspect;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0);
  glDepthFunc(GL_LEQUAL);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

 /*	glBindTexture(GL_TEXTURE_2D, texobjs[i]);*/

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DITHER); /* Nearest , linear , dither */
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DITHER);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGBA,
		            GL_UNSIGNED_BYTE, &terrainpic[0][0][0]);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glMatrixMode(GL_PROJECTION);
  aspect = ((GLfloat) 640.0/(GLfloat)480.0);


  //  removed coz' Not available in FXGLU.LIB

  //  gluPerspective(45, aspect, 0, 20);


  glMatrixMode(GL_MODELVIEW);
}


main( int argc, char *argv[] )
{
  tkInitPosition(0, 0, 640, 480);
  tkInitDisplayMode( TK_RGB | TK_DEPTH | TK_DOUBLE | TK_DIRECT );

  if (tkInitWindow("Mountain") == GL_FALSE)
  {
    tkQuit();
  }

  GlobalMnt=0;
  GlobalSky=0;
  ModZMnt = 0.0f;
  loadpic();

  init();

  tkExposeFunc( reshape );
  tkReshapeFunc( reshape );
//   tkKeyDownFunc( key );
  tkIdleFunc( moveterrain );
  tkDisplayFunc( drawScene );
  tkExec();
}
