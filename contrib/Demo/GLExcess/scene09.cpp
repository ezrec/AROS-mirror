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
#include "scene09.h"

static Texture* h_Text;
static GLuint numtexs=16;
static GLuint width=800;
static GLuint height=600;
static bool init=true;
static GLfloat h_time=0;

static GLfloat h_FogColor[]={ 1.0f, 1.0f, 1.0f, 1.0f };
static int h_y;


static GLfloat h_timer=0.0f;

static GLfloat h_zeta=0.0f;
static GLfloat h_radius=0.0f;

static const int h_num=100;
static const int h_num1=150;

typedef struct
{
	long init;
	GLfloat size;
	GLfloat phase;
	GLfloat rotspd;
	GLfloat spd;
	GLfloat h_y;
	GLfloat a;
	GLfloat a1;
}
h_part;

typedef struct
{
	GLfloat size;
	GLfloat phase;
	GLfloat freq;
	GLfloat amp;
	GLfloat spd;
	GLfloat x,h_y,xpos;
	bool twice;
	int r;
	int g;
	int b;
	int a;

	long init;
}
h_part1;

static h_part parts[h_num];
static h_part1 parts1[h_num1];

void h_Clean(void)
{
	h_Text[1].Kill();
	h_Text[2].Kill();
	h_Text[3].Kill();
	h_Text[4].Kill();
	h_Text[5].Kill();
	h_Text[6].Kill();
	h_Text[7].Kill();
	h_Text[8].Kill();
	h_Text[9].Kill();
	h_Text[10].Kill();
	h_Text[11].Kill();
	h_Text[12].Kill();
	h_Text[13].Kill();
	h_Text[14].Kill();
	h_Text[15].Kill();
	delete [] h_Text;
	init=true;
}

void  h_InitGL()
{
	h_Text=new Texture[numtexs];
	h_timer=0.0f;
	h_time=0.0f;
	h_zeta=0.0f;
	h_radius=0.0f;

	h_y=0;h_timer=0.0f;
	h_zeta=0.0f;h_radius=0.0f;
	h_Text[1].Load("data/fallfront1.raw");
	h_Text[2].Load("data/water.raw");
	h_Text[3].Load("data/fallleft1.raw");
	h_Text[4].Load("data/cl.raw");
	h_Text[5].Load("data/fallleftmask.raw");
	h_Text[6].Load("data/fallright.raw");
	h_Text[7].Load("data/fallrightmask.raw");
	h_Text[8].Load("data/floodmask1.raw");
	h_Text[9].Load("data/smoke.raw");
	h_Text[10].Load("data/circlefill.raw");
	h_Text[11].Load("data/cl1.raw");
	h_Text[12].Load("data/noise1.raw");
	h_Text[13].Load("data/circleempty.raw");
	h_Text[14].Load("data/watpt.raw");
	h_Text[15].Load("data/water1.raw");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,25.0f);
	glMatrixMode(GL_MODELVIEW);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);

	glFogf(GL_FOG_MODE,GL_EXP2);
	glFogf(GL_FOG_START,8.5f);
	glFogf(GL_FOG_END,9.0f);
	glFogf(GL_FOG_DENSITY,0.175f);
	h_FogColor[0]=0.0f;
	h_FogColor[1]=0.0f;
	h_FogColor[2]=0.0f;
	glFogfv(GL_FOG_COLOR,h_FogColor);

	glDisable (GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glDisable (GL_LIGHTING);
	glEnable(GL_BLEND);
	glDisable(GL_FOG);

	for (int i=0; i<h_num; i++)
	{
		parts[i].size=.0005*((GLfloat)(rand()%1000));
		parts[i].phase=3.1415+.001*((GLfloat)(rand()%1000));
		parts[i].rotspd=.001*((GLfloat)(rand()%1000));
		parts[i].spd=.001*((GLfloat)(rand()%1000));
		parts[i].h_y=.001*((GLfloat)(rand()%1000));
		parts[i].a=((GLfloat)(rand()%128))/255.0f;
	}
	for (h_y=0; h_y<h_num1; h_y++)
	{
		parts1[h_y].amp=0.0f;
		parts1[h_y].spd=.1+.00025*((GLfloat)(rand()%1000));
		parts1[h_y].size=.5+.0005*((GLfloat)(rand()%1000));
		parts1[h_y].r=64+rand()%64;
		parts1[h_y].g=parts1[h_y].r;
		parts1[h_y].b=parts1[h_y].r+rand()%16;
		parts1[h_y].x=.001*((GLfloat)(rand()%1000));
		parts1[h_y].xpos=.001*((GLfloat)(rand()%1000));
	}
	for (int xs=0; xs<h_num; xs++) parts[xs].init=(long int)h_time;
	for (int xs=0; xs<h_num1; xs++) parts1[xs].init=(long int)h_time;
}

void h_drawquad(GLfloat size)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.5f*size,-0.5f*size,0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.5f*size,-0.5f*size,0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.5f*size,0.5f*size,0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-0.5f*size,0.5f*size,0);
	glEnd();
}

void h_drawquad0(int subdiv,GLfloat fact,GLuint shd)
{
	GLfloat a=3.0f;
	GLfloat b=3.0f;
	glColor4ub(255,255,255,shd);
	glBegin(GL_QUAD_STRIP);
	for (int i=1; i<subdiv; i++)
	{
		glTexCoord2f(1.0f,h_radius+fact*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(.25+.0025*((GLfloat)subdiv/i),a*cos((90.0f/subdiv)*i*2*3.1415/360.0f),b*sin((90.0f/subdiv)*i*2*3.1415/360.0f));
		glTexCoord2f(0.0f,h_radius+fact*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(-.25-.0025*((GLfloat)subdiv/i),a*cos((90.0f/subdiv)*i*2*3.1415/360.0f),b*sin((90.0f/subdiv)*i*2*3.1415/360.0f));
	}
	glEnd();
}

void h_drawquad1(int subdiv,GLfloat fact,GLuint shd,GLfloat offset)
{
	GLfloat a=3.0f;
	GLfloat b=3.0f;
	glColor4ub(255,255,255,shd);
	glBegin(GL_QUAD_STRIP);
	for (int i=1; i<subdiv; i++)
	{
		glTexCoord2f(1.0f+offset+h_radius/5,h_radius+fact*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(.25-.01*((GLfloat)subdiv/i),a*cos((90.0f/subdiv)*i*2*3.1415/360.0f),b*sin((90.0f/subdiv)*i*2*3.1415/360.0f));
		glTexCoord2f(0.0f+offset+h_radius/5,h_radius+fact*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(-.25+.01*((GLfloat)subdiv/i),a*cos((90.0f/subdiv)*i*2*3.1415/360.0f),b*sin((90.0f/subdiv)*i*2*3.1415/360.0f));
	}
	glEnd();
}

void h_drawquad2(GLfloat size,GLfloat shade)
{
	GLfloat shift=h_radius*.75f;
	GLfloat ix=.5f;
	GLfloat iy=.35f;
	glBegin(GL_QUAD_STRIP);
		glTexCoord2f(0.0f*1-cos(shift)*ix, 0.0f*2+sin(2.0f*shift)*iy);
		glColor4f(0,0,0,0);
		glVertex3f(-.5*size,-.5*size,0);
		glTexCoord2f(1.0f*1-cos(shift)*ix, 0.0f*2+sin(2.0f*shift)*iy);
		glVertex3f(.5*size,-.5*size,0);

		glColor4f(1,1,1,shade);
		glTexCoord2f(0.0f*1-cos(shift)*ix, 0.5f*2+sin(2.0f*shift)*iy);
		glVertex3f(-.5*size,0,0);
		glTexCoord2f(1.0f*1-cos(shift)*ix, 0.5f*2+sin(2.0f*shift)*iy);
		glVertex3f(.5*size,0,0);

		glTexCoord2f(0.0f*1-cos(shift)*ix, 1.0f*2+sin(2.0f*shift)*iy);
		glVertex3f(-.5*size,.5*size,0);
		glTexCoord2f(1.0f*1-cos(shift)*ix, 1.0f*2+sin(2.0f*shift)*iy);
		glVertex3f(.5*size,.5*size,0);

	glEnd();
}
void h_drawquad2b(GLfloat size,GLfloat shade)
{
	glBegin(GL_QUAD_STRIP);
		glTexCoord2f(0.0f*2+h_radius/2.5, 0.0f*2+h_radius/2);
		glColor4f(1,1,1,shade);
		glVertex3f(-.5*size,-.5*size,0);
		glTexCoord2f(1.0f*2+h_radius/2.5, 0.0f*2+h_radius/2);
		glVertex3f(.5*size,-.5*size,0);

		glTexCoord2f(0.0f*2+h_radius/2.5, 0.5f*2+h_radius/2);
		glVertex3f(-.5*size,0,0);
		glTexCoord2f(1.0f*2+h_radius/2.5, 0.5f*2+h_radius/2);
		glVertex3f(.5*size,0,0);

		glColor4f(0,0,0,0);
		glTexCoord2f(0.0f*2+h_radius/2.5, 1.0f*2+h_radius/2);
		glVertex3f(-.5*size,.5*size,0);
		glTexCoord2f(1.0f*2+h_radius/2.5, 1.0f*2+h_radius/2);
		glVertex3f(.5*size,.5*size,0);

	glEnd();
}

void h_drawquad3(int subdiv,GLfloat fact,GLuint shd,GLfloat offset)
{
	GLfloat a=3.0f;
	GLfloat b=3.0f;
	glColor4ub(255,255,255,shd);
	glBegin(GL_QUAD_STRIP);
	for (int i=1; i<subdiv; i++)
	{
		glTexCoord2f(1.0f+offset,h_radius+fact*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(.25-.01*((GLfloat)subdiv/i),a*cos((90.0f/subdiv)*i*2*3.1415/360.0f),b*sin((90.0f/subdiv)*i*2*3.1415/360.0f));
		glTexCoord2f(0.0f+offset,h_radius+fact*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(-.25+.01*((GLfloat)subdiv/i),a*cos((90.0f/subdiv)*i*2*3.1415/360.0f),b*sin((90.0f/subdiv)*i*2*3.1415/360.0f));
	}
	glEnd();
}
bool h_DrawGLScene(GLfloat globtime)
{
	if (init) {h_InitGL();init=false;}
	h_time=10*globtime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
	glLoadIdentity();
	glTranslatef(0,1.9+h_zeta/175.0f,-10);
	h_Text[14].Use();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
	for (int zx=0; zx<h_num1; zx++)
	{
		float time;
		time=(h_time-parts1[zx].init);
		glPushMatrix();
		if ((zx%2)==0) glTranslatef(-parts1[zx].xpos/1.5f-parts1[zx].x/4.0f,-.03+parts1[zx].h_y,(GLfloat)(zx%10)/10);
		else glTranslatef(parts1[zx].xpos/1.5f+parts1[zx].x/4.0f,-.03+parts1[zx].h_y,(GLfloat)(zx%10)/10);
		glColor4ub(parts1[zx].r,parts1[zx].g,parts1[zx].b,(unsigned char)(255+255*(((GLfloat)parts1[zx].h_y)/2.5)));
		glTranslatef(0,-.5,0);
		if ((zx%3)==0) glRotatef(180,1,0,0);
		h_drawquad(parts1[zx].amp);
		if (parts1[zx].amp<parts1[zx].spd*parts1[zx].size*5.0f) parts1[zx].amp=.001*time;
		parts1[zx].x=parts1[zx].spd*time/500.0f;
		parts1[zx].h_y=-1.75*parts1[zx].x*parts1[zx].x;

		if (parts1[zx].h_y<-2.5)
		{
			parts1[zx].init=(long int)h_time;

			parts1[zx].spd=.05+.00025*((GLfloat)(rand()%1000));
			parts1[zx].xpos=.001*((GLfloat)(rand()%1000));
			parts1[zx].amp=0.0f;
			parts1[zx].x=0.0f;
			parts1[zx].h_y=0.0f;
			parts1[zx].r=rand()%64;
			parts1[zx].g=parts1[zx].r+rand()%8;
			parts1[zx].b=parts1[zx].g+rand()%16;
		}
		glPopMatrix();
	}

	glLoadIdentity();
	glTranslatef(1,0,-17);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glPushMatrix();
	glScalef(1.5f,1.0f,1.0f);
	glTranslatef(1.55f,1.9f,.1+h_zeta/20.0f);
	glColor4ub(255,255,255,255);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[0]);
	h_Text[1].Use();

	h_drawquad(10.2f);									// FRONT

	glPopMatrix();
	glEnable(GL_BLEND);
//glutSwapBuffers();	h_time+=1;return;
	glPushMatrix();
	glScalef(1.75,2.0,1);
	glTranslatef(-.8,-1.72,h_zeta/20),
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[3]);
	h_Text[4].Use();
	glColor4ub(255,255,255,128);
	h_drawquad1(20,.2f,128,.7);							// FALL
	glPopMatrix();

	glPushMatrix();
	glScalef(1,2,1);
	glTranslatef(-2.1,-1.71,h_zeta/20),
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[10]);
	h_Text[11].Use();
	h_drawquad3(20,.3f,128,.2);							// FALL
	glPopMatrix();

	glPushMatrix();
	glScalef(1,2,1);
	glTranslatef(.1,-1.71,h_zeta/20),
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[10]);
//	h_Text[11].Use();
	h_drawquad3(20,.4f,128,.2);							// FALL
	glPopMatrix();

	glPushMatrix();
	glScalef(3,2,1);
	glTranslatef(-.15,-1.71,h_zeta/20),
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[3]);
	h_Text[4].Use();
	h_drawquad1(20,.6f,128,.5);							// FALL
	glPopMatrix();

	glPushMatrix();
	glScalef(4.5,2,1);
	glTranslatef(-.24,-1.725,h_zeta/20),
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[10]);
	h_Text[11].Use();
	h_drawquad0(20,1.0f,128);								// FALL
	glPopMatrix();

	glPushMatrix();
	glScalef(.5,2.2,1);
	glTranslatef(4,-1.5,h_zeta/20),
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[11]);
	h_Text[12].Use();
	//glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	h_drawquad3(20,.2f,192,0.0);							// FALL
	glPopMatrix();

	glPushMatrix();
	glScalef(.3,3,1);
	glTranslatef(-13,-1.2,h_zeta/20),
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[11]);
	//h_Text[12].Use();
	h_drawquad3(20,.1f,160,.3);							// FALL
	glPopMatrix();

	glPushMatrix();
	//glEnable(GL_FOG);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[1]);
	h_Text[2].Use();
	glColor4ub(255,255,255,128);
	glTranslatef(-1.0,-3.0,4.4);//+h_zeta/20);
	glRotatef(90,1,0,0);
	glScalef(1.55,1.5,1);
	h_drawquad2(7,.25+.1*cos(h_timer/2.0f));				// WATER
	//glRotatef(90,0,0,1);
	glRotatef(180,1,0,0);
	h_Text[15].Use();
	h_drawquad2b(7,.25+.1*sin(h_timer/5.0f));
	glDisable(GL_FOG);
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0,-1,-5+h_zeta/55);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[9]);			// BIG SPOT
	h_Text[10].Use();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glRotatef(90,1,0,0);
	glColor4ub(255,255,255,255);
	h_drawquad(3.5);
	glColor4ub(255,255,255,128);
	h_drawquad(1.2);
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(1.065,-.987,-5.0+h_zeta/55);					// SPOT
	glRotatef(90,1,0,0);
	glColor4ub(255,255,255,128);
	h_drawquad(.25);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[12]);
	h_Text[13].Use();
	for (int p=0; p<5; p++)
	{
		glColor4f(1,1,1,parts[p].a1/2.0f);
		h_drawquad(parts[p].size/1.5f);
	}
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-1.03,-1.027,-5.0+h_zeta/55);				// SPOT
	glRotatef(90,1,0,0);
	glColor4ub(255,255,255,128);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[9]);
	h_Text[10].Use();
	h_drawquad(.15);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[12]);
	h_Text[13].Use();
	for (int p=5; p<10; p++)
	{
		glColor4f(1,1,1,parts[p].a1/2.0f);
		h_drawquad(parts[p].size/1.5f);
	}
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0,-1.0,-5+h_zeta/55);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[8]);			// PARTS
	h_Text[9].Use();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	for (int p=0; p<h_num; p++)
	{
		long time;
		time=(long int)(h_time-parts[p].init);

		glPushMatrix();
		glTranslatef(-.6+1.2f*(GLfloat)p/h_num,parts[p].h_y,0);
		//glColor4ub(255,255,255,parts[p].a);
		parts[p].a1=parts[p].a-((GLfloat)time/11000.0f);
		glColor4f(1.0f,1.0f,1.0f,parts[p].a1);
		if ((p%2)==0)
		{
			glRotatef(parts[p].phase*100+1000*h_radius*parts[p].rotspd,0,0,1);
			//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glRotatef(parts[p].phase*100-1000*h_radius*parts[p].rotspd,0,0,1);
			//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		}
		h_drawquad(parts[p].size);
		glPopMatrix();

		parts[p].h_y=parts[p].spd*(GLfloat)time/6000.0f;//parts[p].spd/10;
		parts[p].size=parts[p].rotspd*(GLfloat)time/2500.0f;//parts[p].rotspd/50;
		//parts[p].a=1.0f
		//if (h_factor<1.0f) parts[p].a--; else
		//parts[p].a-=(int)(.5f+h_factor);

		//if (parts[p].a<0)
		if (parts[p].a1<0.0f)
		{
			parts[p].init=(long int)h_time;

			parts[p].h_y=0.0f;
			parts[p].size=0.0f;
			parts[p].phase=3.1415+.001*((GLfloat)(rand()%1000));
			parts[p].rotspd=.001*((GLfloat)(rand()%1000));
			parts[p].spd=.25f+.00075*((GLfloat)(rand()%1000));
			parts[p].a=((GLfloat)(rand()%128))/255.0f;
		}
	}
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	glScalef(2.0f,.58,1);
	glTranslatef(0,-2.32,-5.4);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[7]);			// MASK
	h_Text[8].Use();
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	h_drawquad(3);
	glPopMatrix();

	glPushMatrix();
	glScalef(2,4,1);
	glTranslatef(-3.0-h_zeta/30,0,2.5+h_zeta/10);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[4]);			// LEFT
	h_Text[5].Use();
	//glBlendFunc(GL_DST_COLOR,GL_ZERO);
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glColor4f(1,1,1,1);
	h_drawquad(3);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[2]);
	h_Text[3].Use();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
	glColor4f(.99,.99,.99,1);
	h_drawquad(3);
	glPopMatrix();

	glPushMatrix();
	glScalef(4,4,1);
	glTranslatef(.35+h_zeta/25,-.2,2+h_zeta/10);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[6]);			// RIGHT
	h_Text[7].Use();
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glColor4f(1,1,1,1);
	h_drawquad(3);
	//glBindTexture(GL_TEXTURE_2D, h_Text[ure[5]);
	h_Text[6].Use();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glColor4f(.99,.99,.99,1);
	h_drawquad(3);
	glPopMatrix();


	if (h_timer<4.0f)
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1,1,1,.5f*(1.0f+cos(h_timer*3.1415/4.0f)));
		h_drawquad(1.2);
		glEnable(GL_TEXTURE_2D);
	}
	if (h_timer>52.0f)
	{
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		GLfloat shader=.5f*(1.0f-cos((h_timer-52.0f)*3.1415/8.0f));
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glDisable(GL_TEXTURE_2D);
		glColor4f(shader,shader,shader,.5f);
		h_drawquad(1.2);
		glEnable(GL_TEXTURE_2D);
	}
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	h_radius=-h_timer/10.0f;

	if (h_timer<3.0f) h_zeta=2.0f*(1.0f-cos(h_timer*3.1415f/6.0f));
	else h_zeta=2.0f+(h_timer-3.0f);

	h_timer=(h_time)/950.0f;
	if (h_timer>60.0f)
	{
		//***************** FINISH
		//h_Clean();
		return false;
	}
	glutSwapBuffers();
	return true;
}
