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
#include "scene10.h"

static Texture* i_Text;
static GLuint numtexs=21;
static GLuint width=800;
static GLuint height=600;
static bool init=true;
static GLfloat i_time=0;

static long i_gettime;

static GLfloat i_timer=0.0f;
static GLfloat i_radius[7];
static GLfloat i_zeta=0.4f;
static GLfloat i_scale;
static GLfloat i_shade;
static GLfloat i_incr=180.0f;

static int i_x;
static void i_rst(int);

static const int i_num=10;
static const int i_numray=100;

typedef struct
{
	long start;
	GLfloat size;
	GLfloat phase;
	GLfloat xspd,yspd;
	GLfloat i_x,y;
	GLfloat r,g,b,a;
	GLfloat i_shade;
	bool up;
}
i_part;

static i_part rays[i_numray];
static int i_alpha[i_num];

void i_InitGL()
{
	i_Text=new Texture[numtexs];
	i_Text[1].Load("data/you.raw");
	i_Text[2].Load("data/youglow.raw");
	i_Text[3].Load("data/gotta.raw");
	i_Text[4].Load("data/gottaglow.raw");
	i_Text[5].Load("data/say.raw");
	i_Text[6].Load("data/sayglow.raw");
	i_Text[7].Load("data/yes.raw");
	i_Text[8].Load("data/yesglow.raw");
	i_Text[9].Load("data/cl.raw");
	i_Text[10].Load("data/text1.raw");
	i_Text[11].Load("data/xp10.raw");
	i_Text[12].Load("data/basic2.raw");
	i_Text[13].Load("data/cl2.raw");
	i_Text[14].Load("data/excess.raw");
	i_Text[15].Load("data/excessglow.raw");
	i_Text[16].Load("data/another.raw");
	i_Text[17].Load("data/anotherglow.raw");
	i_Text[18].Load("data/to.raw");
	i_Text[19].Load("data/toglow.raw");
	i_Text[20].Load("data/esaflr.raw");

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,4.6f);
	glMatrixMode(GL_MODELVIEW);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDisable(GL_DEPTH_TEST);

	for (int i=0; i<7; i++) i_radius[i]=-1.5f;

	for (int i=0;i<i_numray;i++) i_rst(i);

	for (int i=0;i<i_num;i++) i_alpha[i]=(int)(128-12.8*(GLfloat)i/i_num);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glDisable (GL_CULL_FACE);
	glEnable(GL_POINT_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
	glEnable(GL_BLEND);
}

void i_drawquad(GLfloat size)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.5f*size,0.5f*size,0.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-0.5f*size,0.5f*size,0.0f);
	glEnd();
}

void i_drawtqd(GLfloat size,GLfloat off,int a,int r,int g,int b)
{
	glBegin(GL_QUADS);
		glColor4ub(r,g,b,a);
		glTexCoord2f(0.0f+off, 0.0f);
		glVertex3f(-0.5f*size,-0.5f*size,0.0f);

		glColor4ub(0,0,0,0);
		glTexCoord2f(0.5f+off, 0.0f);
		glVertex3f(0.5f*size,-0.5f*size,0.0f);

		glTexCoord2f(0.5f+off, 1.0f);
		glVertex3f(0.5f*size,0.5f*size,0.0f);

		glColor4ub(a,a,a,a);
		glTexCoord2f(0.0f+off, 1.0f);
		glVertex3f(-0.5f*size,0.5f*size,0.0f);
	glEnd();
}

void i_drawtqd1(GLfloat size,GLfloat off,int a,int r,int g,int b)
{
	glBegin(GL_QUADS);
		glColor4ub(0,0,0,0);
		glTexCoord2f(0.0f+off, 0.0f);
		glVertex3f(-0.5f*size,-0.5f*size,0.0f);

		glColor4ub(r,g,b,a);
		glTexCoord2f(0.5f+off, 0.0f);
		glVertex3f(0.5f*size,-0.5f*size,0.0f);

		glTexCoord2f(0.5f+off, 1.0f);
		glVertex3f(0.5f*size,0.5f*size,0.0f);

		glColor4ub(0,0,0,0);
		glTexCoord2f(0.0f+off, 1.0f);
		glVertex3f(-0.5f*size,0.5f*size,0.0f);
	glEnd();
}

void i_drawtri(GLfloat r,GLfloat g,GLfloat b,GLfloat a,GLfloat size)
{
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(r/2,g/2,b/2,a);
	glVertex3f(0,0,0);
	glColor4f(0,0,0,0);
	glVertex3f(0,.5*size,7);
	glColor4f(r,g,b,a);
	glVertex3f(0,0,7);
	glColor4f(0,0,0,0);
	glVertex3f(0,-.5*size,7);
	glEnd();
}

void i_Clean(void)
{
	i_Text[1].Kill();
	i_Text[2].Kill();
	i_Text[3].Kill();
	i_Text[4].Kill();
	i_Text[5].Kill();
	i_Text[6].Kill();
	i_Text[7].Kill();
	i_Text[8].Kill();
	i_Text[9].Kill();
	i_Text[10].Kill();
	i_Text[11].Kill();
	i_Text[12].Kill();
	i_Text[13].Kill();
	i_Text[14].Kill();
	i_Text[15].Kill();
	i_Text[16].Kill();
	i_Text[17].Kill();
	i_Text[18].Kill();
	i_Text[19].Kill();
	i_Text[20].Kill();
	delete [] i_Text;
	init=true;
}

void i_rst(int i_x)
{
	rays[i_x].size=.25f+.0015f*((GLfloat)(rand()%1000));
	rays[i_x].phase=.180*(GLfloat)(rand()%1000);
	rays[i_x].xspd=.1f+.001f*((GLfloat)(rand()%1000));
	rays[i_x].yspd=.1f+.001f*((GLfloat)(rand()%1000));
	rays[i_x].i_x=0.0f;
	rays[i_x].y=0.0f;
	rays[i_x].r=.001f*((GLfloat)(rand()%1000));
	rays[i_x].g=.001f*((GLfloat)(rand()%1000));
	rays[i_x].b=.001f*((GLfloat)(rand()%1000));
	rays[i_x].a=.0005f*((GLfloat)(rand()%1000));
	rays[i_x].i_shade=0.0f;
	rays[i_x].up=true;
	rays[i_x].start=i_gettime;
}
bool i_DrawGLScene(GLfloat globtime)
{
	if (init) {i_InitGL();init=false;}
	i_time=10*globtime;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glLoadIdentity();
	glTranslatef(0,0,-5+i_zeta);

	glEnable(GL_TEXTURE_GEN_S);						///////////////////// STROBE
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glBindTexture(GL_TEXTURE_2D, texture[9]);
	i_Text[10].Use();
	glPushMatrix();
	glRotatef(45*(i_radius[0]+i_radius[1]),1,0,0);
	glRotatef(45*(i_radius[1]+i_radius[2])+10*i_timer,0,1,0);
	glRotatef(45*(i_radius[2]+i_radius[3]),0,0,1);
	glRotatef(45*(i_radius[3]+i_radius[4]),1,0,1);
	glRotatef(45*(i_radius[4]+i_radius[5]),1,1,0);
	glRotatef(45*(i_radius[5]+i_radius[6]),0,1,1);
	glScalef(.05,.05,.05);
	glColor4ub(255,255,255,192+rand()%63);
	glutSolidDodecahedron();
	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	//glBindTexture(GL_TEXTURE_2D, texture[10]);
	i_Text[11].Use();
	glColor4f(cos(i_radius[0])*cos(i_radius[0])+cos(i_radius[3])*cos(i_radius[3])+cos(i_radius[4])*cos(i_radius[4])+cos(i_radius[5])*cos(i_radius[5]),
	cos(i_radius[1])*cos(i_radius[1])+cos(i_radius[4])*cos(i_radius[4]),
	cos(i_radius[2])*cos(i_radius[2])+cos(i_radius[3])*cos(i_radius[3])+cos(i_radius[6])*cos(i_radius[6])+cos(i_radius[6])*cos(i_radius[6]),
	.75f);
	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, texture[11]);
	i_Text[12].Use();
	i_drawquad(.75+.5*cos(2*(i_radius[0]+i_radius[1]+i_radius[2]+i_radius[3]+i_radius[4]+i_radius[5]+i_radius[6])));
	glRotatef(10*(i_radius[0]+i_radius[1]+i_radius[2]+i_radius[3]+i_radius[4]+i_radius[5]+i_radius[6]),0,0,1);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	//glBindTexture(GL_TEXTURE_2D, texture[10]);
	i_Text[11].Use();
	i_drawquad(.5+.25*sin(i_radius[0]+i_radius[1]+i_radius[2]+i_radius[3]+i_radius[4]+i_radius[5]+i_radius[6]));
	glPopMatrix();
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	if ((i_radius[0]>-1.495)&&(i_radius[0]<1.495))
	{
	glPushMatrix();													// YOU RAY
	if (i_radius[0]<-1.0f) i_shade=1.0f+2*(i_radius[0]+1.0f); else
	if (i_radius[0]>1.0f) i_shade=1.0f-2*(i_radius[0]-1.0f); else
	i_shade=1.0f;
	glRotatef(60*i_radius[0],0,1,0);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, texture[8]);
	i_Text[9].Use();
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.5f,0.162f,0.067f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,0,0);
	glColor4ub(0,0,0,0);
	glVertex3f(0,2.25+.00025*(GLfloat)(rand()%100),3);
	glColor4f(1.0f,0.375f,0.125f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,1.8,3);
	glColor4ub(0,0,0,0);
	glVertex3f(0,.9+.00025*(GLfloat)(rand()%100),3);
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	}
	///////////////////////////////////////////////////// YOU

	glTranslatef(0,1.25,0);
	if ((i_radius[0]>-1.495)&&(i_radius[0]<1.495))
	{
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glPushMatrix();
		glTranslatef(i_radius[0]*.5,0,0);
		glScalef(4,1.5,1);
		glColor4ub((int) ((GLfloat)255*i_shade),(int) ((GLfloat)255*i_shade),(int) ((GLfloat)255*i_shade),255);
		//glBindTexture(GL_TEXTURE_2D, texture[0]);
		i_Text[1].Use();
		i_drawquad(1);
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, texture[1]);
	i_Text[2].Use();
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(128-64*(1+i_radius[0]));
		glPushMatrix();
		glTranslatef(1+i_radius[0]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1.5,1);
		i_scale=.5+i_radius[0]/2;
		i_drawtqd(1,i_scale,(int) ((GLfloat)i_alpha[i_x]*i_shade),255,160,64);
		glPopMatrix();
	}
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(64*(1+i_radius[0]));
		glPushMatrix();
		glTranslatef(-1+i_radius[0]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1.5,1);
		i_scale=i_radius[0]/2;
		i_drawtqd1(1,i_scale,(int) ((GLfloat)i_alpha[i_x]*i_shade),255,160,64);
		glPopMatrix();
	}
	}

	if ((i_radius[1]>-1.495)&&(i_radius[1]<1.495))
	{
	glPushMatrix();								// GOTTA RAY
	if (i_radius[1]<-1.0f) i_shade=1.0f+2*(i_radius[1]+1.0f); else
	if (i_radius[1]>1.0f) i_shade=1.0f-2*(i_radius[1]-1.0f); else
	i_shade=1.0f;
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glBindTexture(GL_TEXTURE_2D, texture[8]);
	i_Text[9].Use();
	glLoadIdentity();
	glTranslatef(0,0,-5+i_zeta);
	glRotatef(60*i_radius[1],0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.067f,0.5f,0.162f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,0,0);
	glColor4ub(0,0,0,0);
	glVertex3f(0,1+.00025*(GLfloat)(rand()%100),5);
	glColor4f(0.125f,1.0f,0.375f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,.5,5);
	glColor4ub(0,0,0,0);
	glVertex3f(0,-.05+.00025*(GLfloat)(rand()%100),5);
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	}

	///////////////////////////////////////////////////// GOTTA

	glTranslatef(0,-1,0);
	if ((i_radius[1]>-1.495)&&(i_radius[1]<1.495))
	{
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glPushMatrix();
		glTranslatef(i_radius[1]*.5,0,0);
		glScalef(4,1,1);
		glColor4ub((int) ((GLfloat)255*i_shade),(int) ((GLfloat)255*i_shade),(int) ((GLfloat)255*i_shade),255);
		//glBindTexture(GL_TEXTURE_2D, texture[2]);
		i_Text[3].Use();
		i_drawquad(1);
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, texture[3]);
	i_Text[4].Use();
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(128-64*(1+i_radius[1]));
		glPushMatrix();
		glTranslatef(1+i_radius[1]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1,1);
		i_scale=.5+i_radius[1]/2;
		i_drawtqd(1,i_scale,(int) ((GLfloat)i_alpha[i_x]*i_shade),64,255,160);
		glPopMatrix();
	}
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(64*(1+i_radius[1]));
		glPushMatrix();
		glTranslatef(-1+i_radius[1]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1,1);
		i_scale=i_radius[1]/2;
		i_drawtqd1(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),64,255,160);
		glPopMatrix();
	}
	}

	if ((i_radius[3]>-1.495)&&(i_radius[3]<1.495))
	{
	glPushMatrix();								// YES RAY
	if (i_radius[3]<-1.0f) i_shade=1.0f+2*(i_radius[3]+1.0f); else
	if (i_radius[3]>1.0f) i_shade=1.0f-2*(i_radius[3]-1.0f); else
	i_shade=1.0f;
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glBindTexture(GL_TEXTURE_2D, texture[8]);
	i_Text[9].Use();
	glLoadIdentity();
	glTranslatef(0,0,-5+i_zeta);
	glRotatef(60*i_radius[3],0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.5f,0.162f,0.5f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,0,0);
	glColor4ub(0,0,0,0);
	glVertex3f(0,-1+.00025*(GLfloat)(rand()%100),3);
	glColor4f(1.0f,0.375f,1.0f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,-1.5,3);
	glColor4ub(0,0,0,0);
	glVertex3f(0,-2.+.00025*(GLfloat)(rand()%100),3);
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	}

	///////////////////////////////////////////////////// YES

	glTranslatef(0,-1.6,0);
	if ((i_radius[3]>-1.495)&&(i_radius[3]<1.495))
	{
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glPushMatrix();
		glTranslatef(i_radius[3]*.5,0,0);
		glScalef(4,1,1);
		glColor4ub((int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),255);
		//glBindTexture(GL_TEXTURE_2D, texture[6]);
		i_Text[7].Use();
		i_drawquad(1);
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, texture[7]);
	i_Text[8].Use();
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(128-64*(1+i_radius[3]));
		glPushMatrix();
		glTranslatef(1+i_radius[3]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1,1);
		i_scale=.5+i_radius[3]/2;
		i_drawtqd(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),255,128,255);
		glPopMatrix();
	}
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(64*(1+i_radius[3]));
		glPushMatrix();
		glTranslatef(-1+i_radius[3]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1,1);
		i_scale=i_radius[3]/2;
		i_drawtqd1(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),255,128,255);
		glPopMatrix();
	}
	}

	if ((i_radius[2]>-1.495)&&(i_radius[2]<1.495))
	{
	glPushMatrix();								// SAY RAY
	if (i_radius[2]<-1.0f) i_shade=1.0f+2*(i_radius[2]+1.0f); else
	if (i_radius[2]>1.0f) i_shade=1.0f-2*(i_radius[2]-1.0f); else
	i_shade=1.0f;
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glBindTexture(GL_TEXTURE_2D, texture[8]);
	i_Text[9].Use();
	glLoadIdentity();
	glTranslatef(0,0,-5+i_zeta);
	glRotatef(60*i_radius[2],0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.067f,0.162f,0.5f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,0,0);
	glColor4ub(0,0,0,0);
	glVertex3f(0,-.25+.00025*(GLfloat)(rand()%100),4);
	glColor4f(0.125f,0.375f,1.0f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,-.7,4);
	glColor4ub(0,0,0,0);
	glVertex3f(0,-1.3+.00025*(GLfloat)(rand()%100),4);
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	}

	///////////////////////////////////////////////////// SAY

	glTranslatef(0,.85,0);
	if ((i_radius[2]>-1.495)&&(i_radius[2]<1.495))
	{
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glPushMatrix();
		glTranslatef(i_radius[2]*.5,0,0);
		glScalef(4,1,1);
		glColor4ub((int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),255);
		//glBindTexture(GL_TEXTURE_2D, texture[4]);
		i_Text[5].Use();
		i_drawquad(1);
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, texture[5]);
	i_Text[6].Use();
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(128-64*(1+i_radius[2]));
		glPushMatrix();
		glTranslatef(1+i_radius[2]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1,1);
		i_scale=.5+i_radius[2]/2;
		i_drawtqd(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),64,160,255);
		glPopMatrix();
	}
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(64*(1+i_radius[2]));
		glPushMatrix();
		glTranslatef(-1+i_radius[2]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1,1);
		i_scale=i_radius[2]/2;
		i_drawtqd1(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),64,160,255);
		glPopMatrix();
	}
	}

	if ((i_radius[4]>-1.495)&&(i_radius[4]<1.495))
	{
	glPushMatrix();								// TO RAY
	if (i_radius[4]<-1.0f) i_shade=1.0f+2*(i_radius[4]+1.0f); else
	if (i_radius[4]>1.0f) i_shade=1.0f-2*(i_radius[4]-1.0f); else
	i_shade=1.0f;
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glBindTexture(GL_TEXTURE_2D, texture[8]);
	i_Text[9].Use();
	glLoadIdentity();
	glTranslatef(0,0,-5+i_zeta);
	glRotatef(60*i_radius[4],0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.5f,0.162f,0.067f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,0,0);
	glColor4ub(0,0,0,0);
	glVertex3f(0,2.5+.00025*(GLfloat)(rand()%100),4);
	glColor4f(0.5f,0.162f,0.067f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,1.75,4);
	glColor4ub(0,0,0,0);
	glVertex3f(0,1+.00025*(GLfloat)(rand()%100),4);
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	}

	///////////////////////////////////////////////////// TO

	glTranslatef(0,1.5,0);
	if ((i_radius[4]>-1.495)&&(i_radius[4]<1.495))
	{
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glPushMatrix();
		glTranslatef(i_radius[4]*.5,0,0);
		glScalef(4,1.5,1);
		glColor4ub((int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),255);
		//glBindTexture(GL_TEXTURE_2D, texture[17]);
		i_Text[18].Use();
		i_drawquad(1);
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, texture[18]);
	i_Text[19].Use();
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(128-64*(1+i_radius[4]));
		glPushMatrix();
		glTranslatef(1+i_radius[4]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1.5,1);
		i_scale=.5+i_radius[4]/2;
		i_drawtqd(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),255,160,64);
		glPopMatrix();
	}
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(64*(1+i_radius[4]));
		glPushMatrix();
		glTranslatef(-1+i_radius[4]*2.5,0,.5*(GLfloat)i_x/i_num);
		glScalef(2,1.5,1);
		i_scale=i_radius[4]/2;
		i_drawtqd1(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),255,160,64);
		glPopMatrix();
	}
	}

	if ((i_radius[5]>-1.495)&&(i_radius[5]<1.495))
	{
	glPushMatrix();								// ANOTHER RAY
	if (i_radius[5]<-1.0f) i_shade=1.0f+2*(i_radius[5]+1.0f); else
	if (i_radius[5]>1.0f) i_shade=1.0f-2*(i_radius[5]-1.0f); else
	i_shade=1.0f;
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glBindTexture(GL_TEXTURE_2D, texture[8]);
	i_Text[9].Use();
	glLoadIdentity();
	glTranslatef(0,0,-5+i_zeta);
	glRotatef(30*i_radius[5],0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.5f,0.162f,0.067f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,0,0);
	glColor4ub(0,0,0,0);
	glVertex3f(0,-.25+.00025*(GLfloat)(rand()%100),4);
	glColor4f(0.5f,0.162f,0.067f,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,-.7,4);
	glColor4ub(0,0,0,0);
	glVertex3f(0,-1.3+.00025*(GLfloat)(rand()%100),4);
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	}

	///////////////////////////////////////////////////// ANOTHER

	glTranslatef(0,-2,0);
	if ((i_radius[5]>-1.495)&&(i_radius[5]<1.495))
	{
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glPushMatrix();
		glTranslatef(i_radius[5]*.5,0,0);
		glScalef(4,.5,1);
		glColor4ub((int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),255);
		//glBindTexture(GL_TEXTURE_2D, texture[15]);
		i_Text[16].Use();
		i_drawquad(1);
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, texture[16]);
	i_Text[17].Use();
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(128-64*(1+i_radius[5]));
		glPushMatrix();
		glTranslatef(1+i_radius[5]*2.5,0,.25*(GLfloat)i_x/i_num);
		glScalef(2,.5,1);
		i_scale=.5+i_radius[5]/2;
		i_drawtqd(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),255,96,96);
		glPopMatrix();
	}
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(64*(1+i_radius[5]));
		glPushMatrix();
		glTranslatef(-1+i_radius[5]*2.5,0,.25*(GLfloat)i_x/i_num);
		glScalef(2,.5,1);
		i_scale=i_radius[5]/2;
		i_drawtqd1(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),255,96,96);
		glPopMatrix();
	}
	}

	if ((i_radius[6]>-1.495)&&(i_radius[6]<1.495))
	{
	glPushMatrix();								// EXCESS RAY
	if (i_radius[6]<-1.0f) i_shade=1.0f+2*(i_radius[6]+1.0f); else
	if (i_radius[6]>1.0f) i_shade=1.0f-2*(i_radius[6]-1.0f); else
	i_shade=1.0f;
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glBindTexture(GL_TEXTURE_2D, texture[8]);
	i_Text[9].Use();
	glLoadIdentity();
	glTranslatef(0,0,-5+i_zeta);
	glRotatef(30*i_radius[6],0,1,0);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.067f*2,0.162f*2,0.5f*2,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,0,0);
	glColor4ub(0,0,0,0);
	glVertex3f(0,.35+.00025*(GLfloat)(rand()%100),3);
	glColor4f(0.125f*2,0.375f*2,1.0f*2,i_shade*(0.625f+0.0025f*((GLfloat)(rand()%100))));
	glVertex3f(0,0,3);
	glColor4ub(0,0,0,0);
	glVertex3f(0,-.35+.00025*(GLfloat)(rand()%100),3);
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	}

	///////////////////////////////////////////////////// EXCESS

	glTranslatef(0,1,0);
	if ((i_radius[6]>-1.495)&&(i_radius[6]<1.495))
	{

	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glPushMatrix();
		glTranslatef(-i_radius[6]*1.5,0,.5);
		glScalef(8,2,1);
		glColor4ub((int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),(int)((GLfloat)255*i_shade),255);
		//glBindTexture(GL_TEXTURE_2D, texture[13]);
		i_Text[14].Use();
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
		i_drawquad(1);
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, texture[14]);
	i_Text[15].Use();
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(128-64*(1+i_radius[6]));
		glPushMatrix();
		glTranslatef(2+i_radius[6]*2.5,0,.5+.75*(GLfloat)i_x/i_num);
		glScalef(4,2,1);
		i_scale=.5+i_radius[6]/2;
		i_drawtqd(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),96,128,255);
		glPopMatrix();
	}
	for (i_x=0; i_x<i_num; i_x++)
	{
		i_alpha[i_x]=(int)(64*(1+i_radius[6]));
		glPushMatrix();
		glTranslatef(-2+i_radius[6]*2.5,0,.5+.75*(GLfloat)i_x/i_num);
		glScalef(4,2,1);
		i_scale=i_radius[6]/2;
		i_drawtqd1(1,i_scale,(int)((GLfloat)i_alpha[i_x]*i_shade),96,128,255);
		glPopMatrix();
	}

	}
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTranslatef(0,0,0);				////////////////////////////////////////	RAYS
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, texture[8]);
	i_Text[9].Use();

	for (int r=0; r<i_numray; r++)
	{
		GLfloat time=(GLfloat)(i_gettime-rays[r].start)/25.0f;
		glPushMatrix();
		int sign;
		if ((r%2)==0) sign=1; else sign=-1;
		glRotatef(sign*rays[r].phase+sign*rays[r].i_x,1,0,0);
		glRotatef(sign*rays[r].phase+sign*rays[r].y,0,1,0);
		//if (((rays[r].phase+rays[r].i_x>-90)&&(rays[r].phase+rays[r].i_x<90))&&((rays[r].phase+rays[r].y>-90)&&(rays[r].phase+rays[r].y<90)))
		i_drawtri(rays[r].r,rays[r].g,rays[r].b,rays[r].i_shade+.00005*((GLfloat)(rand()%1000)),rays[r].size);
		rays[r].i_x=rays[r].xspd*time;
		rays[r].y=rays[r].yspd*time;
		if (time*(rays[r].phase+10.0f)/3000.0f<2.0f*3.1415f) rays[r].i_shade=rays[r].a*(1.0f-cos(time*(rays[r].phase+10.0f)/3000.0f))/2.0f;
		else i_rst(r);
		glPopMatrix();
	}

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, texture[12]);
	i_Text[13].Use();
	glColor4f(1,1,1,.15f);
	glPushMatrix();
	glScalef(3.0f+3.0f*(-cos(i_timer/2.5f)+1.0f),3.0f+3.0f*(-cos(i_timer/2.5f)+1.0f),1);
	glRotatef(i_incr,0,0,1);
	i_drawquad(1);
	glPopMatrix();
	glPushMatrix();
	glScalef(3.0f+3.0f*(cos(i_timer/2.5f)+1.0f),3.0f+3.0f*(cos(i_timer/2.5f)+1.0f),1);
	glRotatef(-i_incr,0,0,1);
	i_drawquad(1);
	glPopMatrix();
	//i_incr+=1.0f;
	glPopMatrix();
	i_timer=((GLfloat)(i_gettime))/500.0f;
	i_incr=i_timer*10.0f;

	if (i_timer<4.0f)
	{
		GLfloat shader=1.0f-i_timer/4.0f;
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(shader,shader,shader,.5);
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	if (i_timer>102.0f)
	{
		GLfloat shader=(i_timer-102.0f)/6.0f;
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(shader,shader,shader,.5);
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}






	if ((i_timer>3.5f)&&(i_timer<6.5f))
	{
		glLoadIdentity();
		glDisable(GL_TEXTURE_2D);
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,.5f-.5f*cos((i_timer-3.5f)*3.1415f/1.5f));
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	if ((i_timer>91.5f)&&(i_timer<97.5f))
	{
		glLoadIdentity();
		glDisable(GL_TEXTURE_2D);
		glTranslatef(0,0,-1.0f);
		if (i_timer<93.5f) glColor4f(1,1,1,.5f-.5f*cos((i_timer-91.5f)*3.1415f/2.0f));
		else glColor4f(1,1,1,.5f+.5f*cos((i_timer-93.5f)*3.1415f/4.0f));
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}


	if (i_timer>95.5f)
	{
	float i_offset=96.5f;
	if ((i_timer>i_offset)&&(i_timer<i_offset+1.0f))
	{
		float alphaval=1.0f-sin((i_timer-i_offset)*3.1415f/2.0f);
		i_Text[1].Use();
		for (int rep=1; rep<5; rep++)
		{
		glLoadIdentity();
		glTranslatef(0,.5,-1.0f+(float)(rep)/5.0f+3.0f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval/rep);
		glScalef(2.0f,.75f,1);
		i_drawquad(1);
		}
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,alphaval/2.0f);
		glDisable(GL_TEXTURE_2D);
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	i_offset=97.0f;
	if ((i_timer>i_offset)&&(i_timer<i_offset+1.0f))
	{
		float alphaval=1.0f-sin((i_timer-i_offset)*3.1415f/2.0f);
		i_Text[3].Use();
		for (int rep=1; rep<5; rep++)
		{
		glLoadIdentity();
		glTranslatef(0,.166,-1.0f+(float)(rep)/5.0f+3.0f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval/rep);
		glScalef(2.0f,.5f,1);
		i_drawquad(1);
		}
		/*glLoadIdentity();
		glTranslatef(0,.166,-1.0f+3.0f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval);
		glScalef(2.0f,.5f,1);
		i_drawquad(1);*/
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,alphaval/2.0f);
		glDisable(GL_TEXTURE_2D);
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	i_offset=97.7f;
	if ((i_timer>i_offset)&&(i_timer<i_offset+1.0f))
	{
		float alphaval=1.0f-sin((i_timer-i_offset)*3.1415f/2.0f);
		i_Text[5].Use();
		for (int rep=1; rep<5; rep++)
		{
		glLoadIdentity();
		glTranslatef(0,-.166,-1.0f+(float)(rep)/5.0f+3.0f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval/rep);
		glScalef(2.0f,.55f,1);
		i_drawquad(1);
		}
		/*glLoadIdentity();
		glTranslatef(0,-.166,-1.0f+3.0f*(-i_timer+i_offset));

		glColor4f(1,1,1,alphaval);
		glScalef(2.0f,.55f,1);
		i_drawquad(1);
		*/
		  glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,alphaval/2.0f);
		glDisable(GL_TEXTURE_2D);
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	i_offset=98.5f;
	if ((i_timer>i_offset)&&(i_timer<i_offset+2.0f))
	{
		float alphaval=1.0f-sin((i_timer-i_offset)*3.1415f/4.0f);
		i_Text[7].Use();
		for (int rep=1; rep<5; rep++)
		{
		glLoadIdentity();
		glTranslatef(0,-.5,-1.5f+(float)(rep)/5.0f+1.5f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval/rep);
		glScalef(2.0f,.55f,1);
		i_drawquad(1);
		}
		/*
		glLoadIdentity();
		glTranslatef(0,-.5,-1.0f+3.0f*(-i_timer+i_offset));

		glColor4f(1,1,1,alphaval);
		glScalef(2.0f,.55f,1);
		i_drawquad(1);
		*/
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,alphaval/2.0f);
		glDisable(GL_TEXTURE_2D);
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	i_offset=101.5f;
	if ((i_timer>i_offset)&&(i_timer<i_offset+1.0f))
	{
		float alphaval=1.0f-sin((i_timer-i_offset)*3.1415f/2.0f);
		i_Text[18].Use();
		for (int rep=1; rep<5; rep++)
		{
		glLoadIdentity();
		glTranslatef(0,.25,-1.0f+(float)(rep)/5.0f+3.0f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval/rep);
		glScalef(1.5f,.55f,1);
		i_drawquad(1);
		}
		/*
		glLoadIdentity();
		glTranslatef(0,.25,-1.0f+3.0f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval);
		glScalef(1.5f,.55f,1);
		i_drawquad(1);
		*/
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,alphaval/2.0f);
		glDisable(GL_TEXTURE_2D);
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	i_offset=102.2f;
	if ((i_timer>i_offset)&&(i_timer<i_offset+1.0f))
	{
		float alphaval=1.0f-sin((i_timer-i_offset)*3.1415f/2.0f);
		i_Text[16].Use();
		for (int rep=1; rep<5; rep++)
		{
		glLoadIdentity();
		glTranslatef(0,-.25,-1.0f+(float)(rep)/5.0f+3.0f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval/rep);
		glScalef(2.0f,.25f,1);
		i_drawquad(1);
		}
		/*
		glLoadIdentity();
		glTranslatef(0,-.25,-1.0f+3.0f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval);
		glScalef(2.0f,.25f,1);
		i_drawquad(1);
		*/
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,alphaval/2.0f);
		glDisable(GL_TEXTURE_2D);
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	i_offset=103.5f;
	if ((i_timer>i_offset)&&(i_timer<i_offset+4.0f))
	{
		float alphaval=1.0f-sin((i_timer-i_offset)*3.1415f/8.0f);
		i_Text[14].Use();
		for (int rep=1; rep<5; rep++)
		{
		glLoadIdentity();
		glTranslatef(0,0,-1.0f+(float)(rep)/5.0f-1.5f*sqrt(i_timer-i_offset));
		glColor4f(1,1,1,alphaval/rep);
		glScalef(2.0f,.65f,1);
		i_drawquad(1);
		}
		/*
		glLoadIdentity();
		glTranslatef(0,0,-2.0f);//+1.2f*(-i_timer+i_offset));
		glColor4f(1,1,1,alphaval);
		glScalef(2.0f,.6f,1);
		i_drawquad(1);
		*/
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,alphaval/2.0f);
		glDisable(GL_TEXTURE_2D);
		i_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	}

	if (i_radius[0]<1.495f) i_radius[0]=-1.5f+((GLfloat)(i_gettime))/4000.0f;
	for (int i=1; i<7; i++)
	{
		//if (benchmode) {if ((i_radius[i-1]>0)&&(i_radius[i]<.745f)) i_radius[i]=-1.5f+((GLfloat)(i_gettime-limit-6000*i))/2000.0f;}
		//	else
		{if ((i_radius[i-1]>0)&&(i_radius[i]<1.495f)) i_radius[i]=-1.5f+((GLfloat)(i_gettime-6000*i))/4000.0f;}
	}

	if (i_timer>108.0f)
	{
		//********************* FINISH
		//i_Clean();
		return false;
	}

	i_gettime=(long int)i_time;
	glutSwapBuffers();
	return true;
}
