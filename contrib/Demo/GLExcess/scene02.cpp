/*
GLExcess v1.0 Demo
Copyright (C) 2001-2003 Paolo Martella
                                                                                                                                                                    
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
                                                                                                                                                                    
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Texture.h"
#include "scene02.h"

static GLuint width=800;
static GLuint height=600;
static bool init=true;
static bool first=true;
static GLfloat a_time=0.0f;

static Texture* a_Text;
static GLuint numtexs=5;

static long a_gets=0;
static int a_timecyc;

static bool switcher=false;
static float gendep=1.55;
static const int size=64;
static float norm[size][size][3];
static float a_points[size][size][3];
static float camera[3]={-12.8,12.8,5};
static float cameraray[3];
static float rray[3];
static float newcoord[size][size][2];
static GLfloat coeff=7.1f;
static int a_x, a_y;
static float a_float_x, a_float_y, a_float_xb, a_float_yb;
static GLfloat	a_xrot;
static GLfloat	a_yrot;
static GLfloat	a_zrot;
static GLfloat a_rad=0.0f;
static GLfloat quantos=-1.0f;
static GLfloat a_zeta=-1.0f;
static GLfloat a_factor=10.0f;

static const int a_num=200;
static int a_waves=0;

typedef struct
{
	GLfloat a_x,a_y,z;
	GLfloat a_mod;
	GLfloat speed,speedlim;
	int r,g,b,a;
	int angle;
	int time;
}
a_part;

static a_part parts[a_num];

static int a_az;
static GLfloat a_counter=0;
static GLfloat a_mod;
static int a_xa,a_ya;

static GLfloat a_diffuse[]= {0.2f, 0.2f, 0.2f, 1.0f};
static GLfloat a_ambient[]= {0.1f, 0.1f, 0.1f, 1.0f};
static GLfloat a_specular[]= {.750f, .750f, .750f, 1.0f};
static GLfloat a_emission[]= {0.2f, 0.2f, 0.2f, 1.0f};

static GLfloat a_shininess = 10.0f;

static GLfloat a_LightAmbient[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
static GLfloat a_LightDiffuse[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
static GLfloat a_LightSpecular[]=	{ .5f, .5f, .5f, 1.0f };
static GLfloat a_LightPosition[]=	{ 0.0f, 8.0f, -20.0f, 1.0f };
static GLfloat a_Sinus[3];


void copy(float vec0[3], float vec1[3])
{
    vec0[0] = vec1[0];
    vec0[1] = vec1[1];
    vec0[2] = vec1[2];
}

void sub(float vec0[3], float vec1[3], float vec2[3])
{
    vec0[0] = vec1[0] - vec2[0];
    vec0[1] = vec1[1] - vec2[1];
    vec0[2] = vec1[2] - vec2[2];
}

void add(float vec0[3], float vec1[3], float vec2[3])
{
    vec0[0] = vec1[0] + vec2[0];
    vec0[1] = vec1[1] + vec2[1];
    vec0[2] = vec1[2] + vec2[2];
}

void scalDiv(float vec[3], float c)
{
    vec[0] /= c; vec[1] /= c; vec[2] /= c;
}

void cross(float vec0[3], float vec1[3], float vec2[3])
{
    vec0[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    vec0[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    vec0[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

void normz(float vec[3])
{
    float c = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    scalDiv(vec, c);
}

void set(float vec[3], float x, float y, float z)
{
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}


void MakeNorm(void)
{
	int i, j;
	float a[3],b[3],c[3];

	for(i=0; i<size; i++)
	for(j=0; j<size; j++)
	{
		if (i!=size-1 && j!=size-1)
		{
			sub(a, a_points[i][j+1], a_points[i][j]);
			sub(b, a_points[i+1][j], a_points[i][j]);
		}
		else
		{
			sub(a, a_points[i][j-1], a_points[i][j]);
			sub(b, a_points[i-1][j], a_points[i][j]);
		}

		cross(c, a, b);
		normz(c);

		if (i==0 && j==size-1)
		{
			sub(a, a_points[i][j-1], a_points[i][j]);
			sub(b, a_points[i+1][j], a_points[i][j]);

			cross(c, a, b);
			normz(c);

			c[0]=-c[0]; c[1]=-c[1]; c[2]=-c[2];
		}

		if (i==size-1 && j==0)
		{
			sub(a, a_points[i-1][j], a_points[i][j]);
			sub(b, a_points[i][j+1], a_points[i][j]);

			cross(c, a, b);
			normz(c);
		}

		copy(norm[i][j], c);
   }
}
void a_setpart();
void a_InitGL()
{
	glDisable(GL_LIGHT0);
	a_time=2.0;
	a_gets=0;a_timecyc=0;
	gendep=1.55;
	cameraray[0]=0;cameraray[1]=0;cameraray[2]=0;
	rray[0]=0;rray[1]=0;rray[2]=0;
	a_x=0;a_y=0;a_float_x=0;a_float_y=0;a_float_xb=0;a_float_yb=0;
	a_xrot=0;a_yrot=0;a_zrot=0;a_rad=0.0f;
	quantos=-1.0f;
	a_zeta=-1.0f;
	a_factor=10.0f;
	a_time=2.0f;
	a_waves=0;
	a_az=0;
	a_counter=0;
	a_mod=0;
	a_xa=0;a_ya=0;

	a_mod=0;
	a_xa=0;a_ya=0;
	a_diffuse[0]=0.2f;a_diffuse[1]=0.2f;a_diffuse[2]=0.2f;a_diffuse[3]=1.0f;
	a_ambient[0]=0.1f;a_ambient[1]=0.1f;a_ambient[2]=0.1f;a_ambient[3]=1.0f;
	a_specular[0]=.75f;a_specular[1]=.75f;a_specular[2]=.75f;a_specular[3]=1.0f;
	a_emission[0]=0.2f;a_emission[1]=0.2f;a_emission[2]=0.2f;a_emission[3]=1.0f;

	a_shininess = 10.0f;

	a_LightAmbient[0]=0.5f;a_LightAmbient[1]=0.5f;a_LightAmbient[2]=0.5f;a_LightAmbient[3]=1.0f;
	a_LightDiffuse[0]=0.5f;a_LightDiffuse[1]=0.5f;a_LightDiffuse[2]=0.5f;a_LightDiffuse[3]=1.0f;
	a_LightSpecular[0]=.5f;a_LightSpecular[1]=.5f;a_LightSpecular[2]=.5f;a_LightSpecular[3]=1.0f;
	a_LightPosition[0]=0.0f;a_LightPosition[1]=8.0f;a_LightPosition[2]=-20.0f;a_LightPosition[3]=1.0f;
	a_Sinus[0]=0;
	a_Sinus[1]=0;
	a_Sinus[2]=0;

	a_xrot=0;
	a_yrot=0;
	a_zrot=0;
	a_counter=0;

	a_rad=0.0f;
	quantos=-1.0f;
	a_zeta=-1.0f;
	a_factor=10.0f;

	a_waves=0;
	a_counter=0;
	coeff=7.1f;
	a_gets=0;
	switcher=false;
	gendep=1.55;

	camera[0]=-12.8;
	camera[1]=12.8;
	camera[2]=5;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	a_Text=new Texture[numtexs];
	a_Text[1].Load("data/logoxs.raw");
	a_Text[2].Load("data/white.raw");
	a_Text[3].Load("data/sun2.raw");
	a_Text[4].Load("data/star.raw");

	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT1);

	glLightfv(GL_LIGHT1,GL_DIFFUSE,a_LightDiffuse);
	glLightfv(GL_LIGHT1,GL_AMBIENT,a_LightAmbient);
	glLightfv(GL_LIGHT1,GL_SPECULAR,a_LightSpecular);
	glLightfv(GL_LIGHT1,GL_POSITION,a_LightPosition);

	glMaterialfv(GL_FRONT,GL_DIFFUSE,a_diffuse);
	glMaterialfv(GL_FRONT,GL_AMBIENT,a_ambient);
	glMaterialfv(GL_FRONT,GL_SPECULAR,a_specular);
	glMaterialf(GL_FRONT,GL_SHININESS,10.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	for (int a_x=0;a_x<size;a_x++)
	{
		for (int a_y=0; a_y<size;a_y++)
		{
			a_points[a_x][a_y][0]=float(a_x/(1.25f*size/32));
			a_points[a_x][a_y][1]=float(a_y/(1.25f*size/32));
			a_points[a_x][a_y][2]=0.0;
		}
	}

	for (int p=0; p<a_num; p++)
	{
		parts[p].r=128+rand()%128;
		parts[p].g=128+rand()%128;
		parts[p].b=128+rand()%128;
		parts[p].a=-1;
		parts[p].angle=rand()%90;
		parts[p].a_mod=0.0f;
		parts[p].speedlim=.005+.0001*((GLfloat)(rand()%1000));
		parts[p].speed=parts[p].speedlim;
		parts[p].a_x=0.0f;
		parts[p].a_y=0.0f;
		parts[p].z=0.0f;
	}

	a_mod=1.0;
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);
	a_setpart();
}

void a_Clean(void)
{
	a_Text[1].Kill();
	a_Text[2].Kill();
	a_Text[3].Kill();
	a_Text[4].Kill();
	delete [] a_Text;
	init=true;
}

void a_setpart(void)
{
	int time=(int)a_time*500-2;//**********************************************
	a_timecyc=time;
	for (int a=0; a<a_num; a++)
	parts[a].time=time;
}

void a_drawquad(GLfloat size)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-.5*size,-.5*size,0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-.5*size,.5*size,0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(.5*size,.5*size,0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(.5*size,-.5*size,0);
	glEnd();
}

void calcul(int xx,int yy)
{
	if ((xx==0)&&(yy==0)) MakeNorm();
	sub(cameraray,camera,a_points[xx][yy]);
	normz(cameraray);
	rray[0]=norm[xx][yy][0]*coeff+cameraray[0];
	rray[1]=norm[xx][yy][1]*coeff+cameraray[1];
	rray[2]=norm[xx][yy][2]*coeff+cameraray[2];
	scalDiv(rray,-1);
	normz(rray);

	GLfloat depth=gendep+a_points[xx][yy][2];
	GLfloat t=depth/rray[2];
	GLfloat mapx=a_points[xx][yy][0]+rray[0]*t;
	GLfloat mapz=a_points[xx][yy][1]+rray[1]*t;

	newcoord[xx][yy][0]=-(mapx-a_points[0][0][0])/25.6;
	newcoord[xx][yy][1]=(mapz-a_points[size][size][1])/25.6;

}

bool a_DrawGLScene(GLfloat globtime)
{
	if (init) {a_InitGL();init=false;}
	a_time=2.0+globtime*.01;
	// MOTION
	if (a_time<10.0f)
	{
		a_zeta=25.0f*cos((3.1415f/2.0f)*(1+a_time/10.0f));
		a_xrot=-45.0f*cos((3.1415f/2.0f)*(1+a_time/10.0f));
	}
	else
	{
		a_xrot=45.0f-30.0f*sin((a_time-10.0f)/20.0f)*sin((a_time-10.0f)/20.0f);
		a_zrot=360.0f*sin((a_time-10.0f)/50.0f)*sin((a_time-10.0f)/50.0f);
		a_zeta=-25.0f+5.0f*sin((a_time-10.0f)/10.0f)*sin((a_time-10.0f)/10.0f);
	}

	if (a_time>90.0f) a_zeta=-20.0f+10.0f*(1.0f-cos((a_time-90.0f)*3.1415f/10.0f));

	if (a_zeta>-2.5f) a_zeta=-2.5f;
	a_factor=(a_xrot/.5+20)/50.0f;

	if (a_mod>0.5f) a_mod=1.0-.03f*(a_time-a_gets); else a_mod=.5-0.015f*(a_time-a_gets);
	if (a_mod<0.0f) a_mod=0.0f;
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
	glTranslatef(0,0,-5);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (a_zeta>-20.0f) glColor4f(0,0,0,-(a_zeta+20.0f)/40.0f+.25f);
	else glColor4f(0,0,0,.25f);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	a_drawquad(6);
	if (first)//a_time<3.01f)
	{
		glDisable(GL_BLEND);
		glColor4ub(255,255,255,255);
		a_drawquad(6);
		glEnable(GL_BLEND);
		first=false;
	}
	if (a_time>95.0f)
	{
		glColor4f(1.0f,1.0f,1.0f,(a_time-95.0f)/1.5f);
		a_drawquad(6);
	}
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_LIGHTING);
	glLoadIdentity();
	if (a_time>30.0) glTranslatef(0.0f,1.5f,a_zeta);
	else glTranslatef(0.0f,.5+.5f*(1.0-cos((a_time-2.0f)*3.1415f/28.0f)),a_zeta);
	glRotatef(-90+2*a_xrot,1.0f,0.0f,0.0f);
	glRotatef(a_yrot,0.0f,1.0f,0.0f);
	glRotatef(a_zrot,0.0f,0.0f,1.0f);


	for (int a_xx=0; a_xx<size; a_xx++) for (int a_yy=0; a_yy<size; a_yy++)
	{
		double raggio;
		double value;
		double arg;

		if (quantos>0.0f)
		{
			raggio=.5*sqrt((double)((a_points[a_xx][a_yy][0]-a_points[size/2][size/2][0])*(a_points[a_xx][a_yy][0]-a_points[size/2][size/2][0])
										  +(a_points[a_xx][a_yy][1]-a_points[size/2][size/2][1])*(a_points[a_xx][a_yy][1]-a_points[size/2][size/2][1])));
			arg=2.5*raggio-quantos*2+30;
			if ((arg<-2*6.28)||(arg>4*6.28)) value=0; else
			value=.05*sin(arg)*sin(arg)*exp(arg/7);
			a_points[a_xx][a_yy][2]=value;
		}
		if (quantos>10)
		{
			raggio=.5*sqrt((double)((a_points[a_xx][a_yy][0]-a_points[48][48][0])*(a_points[a_xx][a_yy][0]-a_points[48][48][0])
									  +(a_points[a_xx][a_yy][1]-a_points[48][48][1])*(a_points[a_xx][a_yy][1]-a_points[48][48][1])));
			arg=2.5*raggio-(quantos-10)*3+30;
			if ((arg<-2*6.28)||(arg>4*6.28)) value=0; else
			value=.025*sin(arg)*sin(arg)*exp(arg/7);
			a_points[a_xx][a_yy][2]+=value;
		}
		if (quantos>24)
		{
			raggio=.5*sqrt((double)((a_points[a_xx][a_yy][0]-a_points[50][22][0])*(a_points[a_xx][a_yy][0]-a_points[50][22][0])
									  +(a_points[a_xx][a_yy][1]-a_points[50][22][1])*(a_points[a_xx][a_yy][1]-a_points[50][22][1])));
			arg=3.0*raggio-(quantos-24)*4+30;
			if ((arg<-2*6.28)||(arg>4*6.28)) value=0; else
			value=.02*sin(arg)*sin(arg)*exp(arg/7);
			a_points[a_xx][a_yy][2]+=value;
		}
		if (quantos>32)
		{
			raggio=.5*sqrt((double)((a_points[a_xx][a_yy][0]-a_points[32][32][0])*(a_points[a_xx][a_yy][0]-a_points[32][32][0])
									  +(a_points[a_xx][a_yy][1]-a_points[32][32][1])*(a_points[a_xx][a_yy][1]-a_points[32][32][1])));
			arg=2.5*raggio-(quantos-32)*3+30;
			if ((arg<0*6.28)||(arg>4*6.28)) value=0; else
			value=.035*sin(arg)*sin(arg)*exp(arg/7);
			a_points[a_xx][a_yy][2]+=value;
		}

		calcul(a_xx,a_yy);
	}
	if (a_time>34.0f) quantos=2.0+(a_time-34.0)/1.5f;

	a_Text[1].Use();
	glMaterialfv(GL_FRONT,GL_DIFFUSE,a_diffuse);
	glMaterialfv(GL_FRONT,GL_AMBIENT,a_ambient);
	glMaterialfv(GL_FRONT,GL_SPECULAR,a_specular);
	glMaterialf(GL_FRONT,GL_SHININESS,10.0f);
	glPushMatrix();
	//glDisable(GL_LIGHTING);
	glScalef(-1,-1,1);
	glColor4f(1,1,1,1);

	glPushMatrix();
	glTranslatef(-12.8,12.8,0);



	glNormal3f(0,0,1);
	for (int cc=0; cc<1; cc++)
	{
		if ((cc%2)==0)
		{
			glScalef(1,-1,1);
			glFrontFace(GL_CCW);
		}
		else
		{
			glScalef(-1,1,1);
			glFrontFace(GL_CW);
		}
		glBegin(GL_QUADS);
		for( a_x = 0; a_x < size-1; a_x++ )
		{
			if (1)//a_x%2==0)
			{
			for( a_y = 0; a_y < size-1; a_y++ )
			{
				glTexCoord2f(newcoord[a_x][a_y][0],newcoord[a_x][a_y][1]);
				//glNormal3f(-norm[a_x][a_y][0],-norm[a_x][a_y][1],-norm[a_x][a_y][2]);
				glVertex3f( a_points[a_x][a_y][0], a_points[a_x][a_y][1], a_points[a_x][a_y][2]);

				glTexCoord2f(newcoord[a_x][a_y+1][0],newcoord[a_x][a_y+1][1]);
				//glNormal3f(-norm[a_x][a_y+1][0],-norm[a_x][a_y+1][1],-norm[a_x][a_y+1][2]);
				glVertex3f( a_points[a_x][a_y+1][0], a_points[a_x][a_y+1][1], a_points[a_x][a_y+1][2]);

				glTexCoord2f(newcoord[a_x+1][a_y+1][0],newcoord[a_x+1][a_y+1][1]);
				//glNormal3f(-norm[a_x+1][a_y+1][0],-norm[a_x+1][a_y+1][1],-norm[a_x+1][a_y+1][2]);
				glVertex3f( a_points[a_x+1][a_y+1][0], a_points[a_x+1][a_y+1][1], a_points[a_x+1][a_y+1][2]);

				glTexCoord2f(newcoord[a_x+1][a_y][0],newcoord[a_x+1][a_y][1]);
				//glNormal3f(-norm[a_x+1][a_y][0],-norm[a_x+1][a_y][1],-norm[a_x+1][a_y][2]);
				glVertex3f( a_points[a_x+1][a_y][0], a_points[a_x+1][a_y][1], a_points[a_x+1][a_y][2]);
			}
			}
			else
			{
			for(a_y=size-2;a_y>=0;a_y--)
			{
				glTexCoord2f(newcoord[a_x][a_y][0],newcoord[a_x][a_y][1]);
				glVertex3f( a_points[a_x][a_y][0], a_points[a_x][a_y][1], a_points[a_x][a_y][2]);

				glTexCoord2f(newcoord[a_x][a_y+1][0],newcoord[a_x][a_y+1][1]);
				glVertex3f( a_points[a_x][a_y+1][0], a_points[a_x][a_y+1][1], a_points[a_x][a_y+1][2]);

				glTexCoord2f(newcoord[a_x+1][a_y+1][0],newcoord[a_x+1][a_y+1][1]);
				glVertex3f( a_points[a_x+1][a_y+1][0], a_points[a_x+1][a_y+1][1], a_points[a_x+1][a_y+1][2]);

				glTexCoord2f(newcoord[a_x+1][a_y][0],newcoord[a_x+1][a_y][1]);
				glVertex3f( a_points[a_x+1][a_y][0], a_points[a_x+1][a_y][1], a_points[a_x+1][a_y][2]);
			}
			}
		}
		glEnd();
	}
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glPopMatrix();
	glDisable(GL_LIGHTING);
	a_Text[4].Use();
	a_counter=a_time*10.0f;

	for (int p=0; p<a_num; p++)
	{
		GLfloat time;
		time=a_time*500-2.0-parts[p].time;//********************************************************
		glPushMatrix();
		glTranslatef(parts[p].a_x,parts[p].a_y,parts[p].z);
		glRotatef(-a_zrot,0,0,1);
		glRotatef(90-2.0f*a_xrot,1,0,0);
		glRotatef(parts[p].angle-135,0,0,1);
		glTranslatef(parts[p].a_mod,0,0);

		if (a_time<20.0f) glColor4ub(parts[p].r,parts[p].g,parts[p].b,(unsigned char)((parts[p].a-(int)((GLfloat)time/8.0f))*(a_time-6.0)/14.0));
		else glColor4ub(parts[p].r,parts[p].g,parts[p].b,(unsigned char)(parts[p].a-(int)((GLfloat)time/8.0f)));

		if (a_time>6.0) a_drawquad(1.125f-.75*p/a_num);
		parts[p].a_mod=parts[p].speed*time/35.0f;
		parts[p].speed=parts[p].speedlim-time/2500000.0f;
		if (parts[p].speed<0.005f) parts[p].speed=0.005f;
		if (parts[p].a-(int)((GLfloat)time/8.0f)<3)

		{
			parts[p].a_x=10.0f*sin(a_counter*4.0f*3.14f/360.0f);
			parts[p].a_y=0.0f+10.0f*sin(a_counter*2*3.14/360.0);
			parts[p].z=a_Sinus[2]=3.0f-2.5f*cos(a_counter*8.0f*3.14f/360.0f);
			parts[p].r=128+rand()%128;
			parts[p].g=128+rand()%128;
			parts[p].b=128+rand()%128;
			parts[p].a=rand()%255;
			parts[p].a_mod=0.0f;
			parts[p].speedlim=.005+.0001*((GLfloat)(rand()%1000));
			parts[p].speed=parts[p].speedlim;
			parts[p].time=(int)(a_time*500-2);//*********************************
		}

		glPopMatrix();
	}
	glPushMatrix();
	a_Sinus[0]=10.0f*sin(a_counter*4.0f*3.14f/360.0f);
	a_Sinus[1]=0.0f+10.0f*sin(a_counter*2*3.14/360.0);
	a_Sinus[2]=3.0f-2.5f*cos(a_counter*8.0f*3.14f/360.0f);
	glTranslatef(a_Sinus[0],a_Sinus[1],a_Sinus[2]);

	glRotatef(-a_zrot,0,0,1);
	glRotatef(90-2*a_xrot,1,0,0);
	glColor4ub(255,128,255,255);
	glColor4ub(128,192,255,255);
	glRotatef(2*a_counter,0,0,1);

	a_LightPosition[0]=0;//a_Sinus[0];//10.0f*sin(a_counter*4.0f*3.14f/360.0f);
	a_LightPosition[1]=0;//a_Sinus[1];//0.0f+10.0f*sin(a_counter*2*3.14/360.0);
	a_LightPosition[2]=0;//a_Sinus[2];//3.0f-2.5f*cos(a_counter*8.0f*3.14f/360.0f);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT1,GL_POSITION,a_LightPosition);
	glDisable(GL_LIGHTING);
	a_Text[2].Use();
	a_drawquad(1.0f+sin(a_counter*12.0f*3.1415f/360.0)+3.1415f/2.0f);
	a_Text[3].Use();
	a_drawquad(3.0f+2*sin(a_counter*6.0f*3.1415f/360.0));
	glPopMatrix();

/*
	a_LightPosition[0]=10.0f*sin(a_counter*4.0f*3.14f/360.0f);
	a_LightPosition[1]=0.0f+10.0f*sin(a_counter*2*3.14/360.0);
	a_LightPosition[2]=3.0f-2.5f*cos(a_counter*8.0f*3.14f/360.0f);
//	glTranslatef(a_LightPosition[0],a_LightPosition[1],a_LightPosition[2]);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT1,GL_POSITION,a_LightPosition);
	*/

//	a_time=2.0f+(1)/500.0f;//************************************


	if (a_time>96.0f)
	{
		//****************************** FINISH
		//a_Clean();
		return false;
	}
	glutSwapBuffers();
	return true;
}
