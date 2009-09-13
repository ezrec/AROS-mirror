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
#include "scene11.h"

static Texture* j_Text;
static GLuint numtexs=19;
static GLuint width=800;
static GLuint height=600;
static bool init=true;
static GLfloat j_time=0;

static GLfloat j_FogColor[]={1.0f,1.0f,1.0f,1.0f};
//static int j_x, j_y;
static GLfloat j_max=0.0f;

static GLfloat j_zeta=-6.7f;
static GLfloat j_radius=0.0f;

static bool th [2];
static const int j_num=150;

typedef struct
{
	GLfloat j_x,j_y,xp;
	GLfloat rad;
	GLfloat phase;
	GLfloat spd;
	int r,g,b,a;
	long init;
}
j_part;

static j_part parts[j_num];

void j_InitGL()
{
	j_Text=new Texture[numtexs];
	j_max=0.0f;
	j_Text[1].Load("data/provaz.raw");
	j_Text[2].Load("data/skygs.raw");
	j_Text[3].Load("data/white.raw");
	j_Text[4].Load("data/moon.raw");
	j_Text[5].Load("data/stars1.raw");
	j_Text[6].Load("data/moonmask.raw");
	j_Text[7].Load("data/spread.raw");
	j_Text[8].Load("data/circle.raw");
	j_Text[9].Load("data/circleempty.raw");
	j_Text[10].Load("data/circlefill.raw");
	j_Text[11].Load("data/noise.raw");
	j_Text[12].Load("data/tail.raw");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	j_Text[13].Load("data/profile.raw");
	j_Text[14].Load("data/star.raw");
	j_Text[15].Load("data/land.raw");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	j_Text[16].Load("data/esaflr.raw");
	j_Text[17].Load("data/credits.raw");
	j_Text[18].Load("data/creditsneg.raw");

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,150.0f);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix

	glShadeModel(GL_SMOOTH);
	glClearColor(0,0,0,0);//.07f, 0.1f, 0.25f, 0.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);

	j_FogColor[0]=1.0f;
	j_FogColor[1]=.8f;
	j_FogColor[2]=.5f;

	glDisable (GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);

	for (int i=0; i<j_num; i++)
	{
		parts[i].j_x=.001*((GLfloat)(rand()%12000));
		if (parts[i].j_x>6.0f) parts[i].j_x-=12.0f;
		parts[i].j_y=.001*((GLfloat)(rand()%5000));
		parts[i].rad=.5+.005*((GLfloat)(rand()%1000));//parts[i].j_y*.001*((GLfloat)(rand()%1000));
		parts[i].phase=.002*((GLfloat)(rand()%1000))*3.1415;
		parts[i].spd=.25+.0025*((GLfloat)(rand()%1000));

		parts[i].xp=.00044*((GLfloat)(rand()%1000));
		if (parts[i].xp>0.22f) parts[i].xp-=.44f;
		parts[i].r=128+rand()%128;
		parts[i].b=parts[i].r;
		parts[i].g=parts[i].r;
		parts[i].a=(int)(parts[i].j_y*51.0f);
	}
	glEnable(GL_POINT_SMOOTH);
	th[0]=true;
	th[1]=true;
}

void j_Clean(void)
{
	j_Text[1].Kill();
	j_Text[2].Kill();
	j_Text[3].Kill();
	j_Text[4].Kill();
	j_Text[5].Kill();
	j_Text[6].Kill();
	j_Text[7].Kill();
	j_Text[8].Kill();
	j_Text[9].Kill();
	j_Text[10].Kill();
	j_Text[11].Kill();
	j_Text[12].Kill();
	j_Text[13].Kill();
	j_Text[14].Kill();
	j_Text[15].Kill();
	j_Text[16].Kill();
	j_Text[17].Kill();
	j_Text[18].Kill();
	delete [] j_Text;
	init=true;
}

void j_drawquad(GLfloat size)
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

void j_drawcred(GLfloat sizew, GLfloat sizeh, GLfloat pos, GLfloat facts)
{
//	if (benchmode) return;
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBegin(GL_QUAD_STRIP);

		glColor4f(0,0,0,0);
		glTexCoord2f(0.0f, 0.0f+pos);
		glVertex3f(-1*sizew,-1*sizeh,0);
		glTexCoord2f(1.0f, 0.0f+pos);
		glVertex3f(1*sizew,-1*sizeh,0);

		glColor4f(1,1,1,1);
		glTexCoord2f(0.0f, facts*.025f+pos);
		glVertex3f(-1*sizew,-.9*sizeh,0);
		glTexCoord2f(1.0f, facts*.025f+pos);
		glVertex3f(1*sizew,-.9*sizeh,0);

		glTexCoord2f(0.0f, facts*.475f+pos);
		glVertex3f(-1*sizew,.9*sizeh,0);
		glTexCoord2f(1.0f, facts*.475f+pos);
		glVertex3f(1*sizew,.9*sizeh,0);

		glColor4f(0,0,0,0);
		glTexCoord2f(0.0f, facts*.5f+pos);
		glVertex3f(-1*sizew,1*sizeh,0);
		glTexCoord2f(1.0f, facts*.5f+pos);
		glVertex3f(1*sizew,1*sizeh,0);

	glEnd();
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void j_drawquad1(int col,GLfloat sizex,GLfloat sizey)
{
	glBegin(GL_QUADS);
		if (col>32) glColor4ub(255,255,255,col); else glColor4ub(255,255,255,32);
		glTexCoord2f(0.0f+j_radius/3, 0.0f+j_radius);
		glVertex3f(-.5*sizex,-.5*sizey,0);
		glTexCoord2f(1.0f*2+j_radius/3, 0.0f+j_radius);
		glVertex3f(-.5*sizex,.5*sizey,0);
		glColor4ub(255,255,255,col/2);
		glTexCoord2f(1.0f*2+j_radius/3, 1.0f*2+j_radius);
		glVertex3f(.5*sizex,.5*sizey,0);
		glTexCoord2f(0.0f+j_radius/3, 1.0f*2+j_radius);
		glVertex3f(.5*sizex,-.5*sizey,0);
	glEnd();
}

void j_drawquad10(int col,GLfloat sizex,GLfloat sizey)
{
	glBegin(GL_QUADS);
		if (col>32) glColor4ub(255,255,255,col); else glColor4ub(255,255,255,32);
		glColor4f(0,0,0,0);
		glVertex3f(-.5*sizex,-.5*sizey,0);
		glVertex3f(-.5*sizex,.5*sizey,0);

		glColor4f(j_FogColor[0],j_FogColor[1],j_FogColor[2],1.75*(GLfloat)col/255.0f);
		glVertex3f(.5*sizex,.5*sizey,0);
		glVertex3f(.5*sizex,-.5*sizey,0);

	glEnd();
}

void j_drawquad2(int col,int shd,GLfloat sizex,GLfloat sizey)
{
	glBegin(GL_QUADS);
		glColor4ub(col,col,col,shd);
		glTexCoord2f(1+0.0f+j_radius/2, 0.0f+j_radius/2);
		glVertex3f(-.5*sizex,-.5*sizey,0);
		glTexCoord2f(1+1.0f*2+j_radius/2, 0.0f+j_radius/2);
		glVertex3f(-.5*sizex,.5*sizey,0);
		glColor4ub(col,col,col,shd/2);
		glTexCoord2f(1+1.0f*2+j_radius/2, 1.0f*2+j_radius/2);
		glVertex3f(.5*sizex,.5*sizey,0);
		glTexCoord2f(1+0.0f+j_radius/2, 1.0f*2+j_radius/2);
		glVertex3f(.5*sizex,-.5*sizey,0);
	glEnd();
}

void j_drawquad3(int col,GLfloat sizex,GLfloat sizey)
{
	glBegin(GL_QUADS);
		glColor4ub(0,0,0,col);
		glTexCoord2f(0.0f-j_radius/10.0f, 0.0f);
		glVertex3f(-.5*sizex,-.5*sizey,0);
		glTexCoord2f(1.0f-j_radius/10.0f, 0.0f);
		glColor4ub(255,255,255,col);
		glVertex3f(-.5*sizex,.5*sizey,0);
		glTexCoord2f(1.0f-j_radius/10.0f, 2.0f);
		glVertex3f(.5*sizex,.5*sizey,0);
		glTexCoord2f(0.0f-j_radius/10.0f, 2.0f);
		glColor4ub(0,0,0,col);
		glVertex3f(.5*sizex,-.5*sizey,0);
	glEnd();
}

void j_drawquad6(int col,GLfloat sizex,GLfloat sizey)
{
	glBegin(GL_QUADS);
		glColor4ub(0,0,0,col);
		glTexCoord2f(0.0f-j_radius*2.0f, 0.0f+j_radius/2.0f);
		glVertex3f(-.5*sizex,-.5*sizey,0);
		glTexCoord2f(1.0f-j_radius*2.0f, 0.0f+j_radius/2.0f);
		glColor4ub(col,col,col,255);
		glVertex3f(-.5*sizex,.5*sizey,0);
		glTexCoord2f(1.0f-j_radius*2.0f, 2.0f+j_radius/2.0f);
		glVertex3f(.5*sizex,.5*sizey,0);
		glTexCoord2f(0.0f-j_radius*2.0f, 2.0f+j_radius/2.0f);
		glColor4ub(0,0,0,col);
		glVertex3f(.5*sizex,-.5*sizey,0);
	glEnd();
}

void j_drawtrail(GLuint thickness, GLint alpha, GLfloat length, GLuint r,GLuint g,GLuint b,GLuint a)
{
	glPushMatrix();
	for (unsigned int p=0; p<thickness; p++)
	{
		glTranslatef(0,-1/length,0);
		glPushMatrix();
		glRotatef(-alpha,1,0,0);
		glRotatef(p*j_radius*10,0,0,1);
		glColor4ub(r,g,b,a);
		j_drawquad(j_max*(p*.2*j_radius/2)+.00002*((GLfloat)(rand()%1000)));
		glPopMatrix();
	}
	glPopMatrix();
}

bool j_DrawGLScene(GLfloat globtime)
{
	if (init) {j_InitGL();init=false;}
	j_time=10*globtime;

	j_radius=-.075f+(j_time)/45000.0f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
if (j_radius>.0f)
{
	if (j_radius<1.5f)
	{
	glLoadIdentity();
	glTranslatef(0.0f,0.75f,j_zeta-2.0f);
	glRotatef(80,1.0f,0.0f,0.0f);
	glRotatef(0,0.0f,1.0f,0.0f);
	glRotatef(-90,0.0f,0.0f,1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[4]);
	j_Text[5].Use();
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0,1.25,-5);
	j_drawquad3((int)(32*j_radius),6.0,2.5);		// STELLE
	glPopMatrix();

	glLoadIdentity();			// LUNA
	glTranslatef(.55f+3.0f*cos(.35+j_radius/2),.4f+1.25*sin(.35+j_radius/2),-5);
	glRotatef(-45,0,0,1);
	glColor4ub(255,255,255,255);
	glBlendFunc(GL_DST_COLOR,GL_ZERO);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[5]);
	//j_Text[6].Use();
	glColor4ub(255,255,255,(unsigned char)(148*(j_radius-.35)));
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D,j_Text[ure[3]);
	j_Text[4].Use();
	j_drawquad(1);

	if ((j_radius-.5f<.053)&&(j_radius-.5f>-.007f))
	{
	glLoadIdentity();
	glTranslatef(-2.5+(j_radius-.5f)*25,1.2f+(j_radius-.5f)*4.7,-5);

	j_Text[12].Use();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glRotatef(100,0,0,1);		// SCIA
	glScalef(.1,sin(55*(j_radius-.5f))*sin(55*(j_radius-.5f)),1);
	glColor4ub(255,255,255,(unsigned char)(64*sin(55*(j_radius-.5f))*sin(55*(j_radius-.5f))));
	j_drawquad(1);
	}

	if ((j_radius<.08)&&(j_radius>.05))
	{
	glLoadIdentity();
	glTranslatef(-1.0+(-.05+j_radius)*50,1.5f-(-.05+j_radius)*9,-5);
	j_Text[12].Use();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glRotatef(80,0,0,1);		// SCIA
	glScalef(.07,.5*sin(100*(-.05+j_radius))*sin(100*(-.05+j_radius)),1);
	glColor4ub(255,255,255,(unsigned char)(255*sin(100*(-.05+j_radius))*sin(100*(-.05+j_radius))));
	j_drawquad(1);
	}

	glLoadIdentity();
	glTranslatef(0.0f,0.75f,j_zeta-2.0f);
	glRotatef(80,1.0f,0.0f,0.0f);
	glRotatef(0,0.0f,1.0f,0.0f);
	glRotatef(-90,0.0f,0.0f,1.0f);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0,1.4,j_zeta-2.0f);
	//glRotatef(-90+j_zrot,0.0f,0.0f,1.0f);
	glRotatef(-90,0,0,1);
	j_drawquad10(100,3,10);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);



	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[0]);
	j_Text[1].Use();
	j_drawquad1(100,10,15);		// NUVOLE
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[1]);
	j_Text[2].Use();

	if (((j_radius>1.482)&&(j_radius<1.484))||
		((j_radius>1.495))||
		((j_radius>1.305)&&(j_radius<1.306))||
		((j_radius>1.3)&&(j_radius<1.3025)))
		j_drawquad2(128+rand()%128,128+rand()%128,10,15);

	else if (j_radius>1.2) j_drawquad2(190,(int)(128-128*(j_radius-1.2)/.3),10,15);
	else j_drawquad2(190,128,10,15);
	glLoadIdentity();
	glTranslatef(1.5f*cos(2.2+j_radius),sin(2.2+j_radius),-3);
	//glBindTexture(GL_TEXTURE_2D,j_Text[ure[2]);
	j_Text[3].Use();
	glPushMatrix();
	glRotatef(j_radius*300,0,0,1);
	glColor4ub(255,255,255,(unsigned char)(255-50*j_radius));
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	if (j_radius<1.1) j_drawquad(1.0-j_radius/1.75f);
	glPopMatrix();

	if (j_radius*3.0f<3.1415)
	{
	glRotatef(j_radius*90,0,0,1);

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[8]);
	j_Text[9].Use();
	glColor4ub(128,160,255,(unsigned char)(32*sin(j_radius*3.0f)));
	glTranslatef(j_radius/1.75,0,0);
	j_drawquad(1.25);
	glPopMatrix();

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[8]);
	j_Text[9].Use();
	glColor4ub(192,48,16,(unsigned char)(80*sin(j_radius*3.0f)));
	glTranslatef(j_radius*1.5f,0,0);
	j_drawquad(.4);
	glPopMatrix();

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[9]);
	j_Text[10].Use();
	glColor4ub(64,192,96,(unsigned char)(100*sin(j_radius*3.0f)));
	glTranslatef(j_radius*1.81f,0,0);
	j_drawquad(.2);
	glColor4ub(255,255,255,(unsigned char)(100*sin(j_radius*3.0f)));
	j_drawquad(.1);
	glPopMatrix();

	glPushMatrix();
	//BindTexture(GL_TEXTURE_2D, j_Text[ure[7]);
	j_Text[8].Use();
	glColor4ub(96,128,192,(unsigned char)(64*sin(j_radius*3.0f)));
	glTranslatef(j_radius*2.5f,0,0);
	j_drawquad(.45);
	glPopMatrix();

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[7]);
	j_Text[8].Use();
	glColor4ub(192,192,160,(unsigned char)(64*sin(j_radius*3.0f)));
	glTranslatef(j_radius*2.1f,0,0);
	j_drawquad(.3);
	glPopMatrix();

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[8]);
	j_Text[9].Use();
	glColor4ub(132,160,148,(unsigned char)(32*sin(j_radius*3.0f)));
	glTranslatef(j_radius*1.25f,0,0);
	j_drawquad(.75);
	glPopMatrix();
	}
	glLoadIdentity();			// LUNA
	glTranslatef(.55f+3.0f*cos(.35+j_radius/2),.4f+1.25*sin(.35+j_radius/2),-5);
	glRotatef(-45,0,0,1);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[6]);
	j_Text[7].Use();
	glColor4ub(255,255,255,(unsigned char)(53.3*j_radius));
	//j_drawquad(3.0f);

	}		//*************************************************************************************************
	else	//*************************************************************************************************
	{
	glLoadIdentity();
	glTranslatef(0.0f,0.75f,j_zeta-2.0f);
	glRotatef(80,1.0f,0.0f,0.0f);
	glRotatef(0,0.0f,1.0f,0.0f);
	glRotatef(-90,0.0f,0.0f,1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[4]);
	j_Text[5].Use();
	glLoadIdentity();
	glTranslatef(0,1.25,-5);
	if (j_radius>2.0f)
	{
		j_drawquad3(255,6.0,2.5);		// STELLE
		j_drawquad3(255,6.0,2.5);		// STELLE
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		//glBindTexture(GL_TEXTURE_2D, j_Text[ure[10]);
		j_Text[11].Use();
		j_drawquad6(192,6.0,2.5);
	}
	else
	{
		j_drawquad3((int)(50+205*(2*(j_radius-1.5))),6.0,2.5);
		j_drawquad3((int)(50+205*(2*(j_radius-1.5))),6.0,2.5);
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		//glBindTexture(GL_TEXTURE_2D, j_Text[ure[10]);
		j_Text[11].Use();
		j_drawquad6((int)(192*(2*(j_radius-1.5))),6.0,2.5);
	}
	glLoadIdentity();			// LUNA
	glTranslatef(.55f+3.0f*cos(.35+j_radius/2),.4f+1.25*sin(.35+j_radius/2),-5);
	glRotatef(-45,0,0,1);
	glColor4ub(255,255,255,255);
	glBlendFunc(GL_DST_COLOR,GL_ZERO);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[5]);
	j_Text[6].Use();
	j_drawquad(1);
	glColor4ub(255,255,255,255);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D,j_Text[ure[3]);
	j_Text[4].Use();
	j_drawquad(1);

	glLoadIdentity();
	glTranslatef(0.0f,0.75f,j_zeta-2.0f);
	glRotatef(80,1.0f,0.0f,0.0f);
	glRotatef(0,0.0f,1.0f,0.0f);
	glRotatef(-90,0.0f,0.0f,1.0f);

	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0,1.4,j_zeta-2.0f);
	//glRotatef(-90+j_zrot,0.0f,0.0f,1.0f);
	glRotatef(-90,0,0,1);
	j_drawquad10(100,3,10);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[0]);
	j_Text[1].Use();
	j_drawquad1(100,10,15);		// NUVOLE
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[1]);
	j_Text[2].Use();
	j_drawquad2(255,64,10,15);			// NUVOLE

	glLoadIdentity();			// LUNA
	glTranslatef(.55f+3.0f*cos(.35+j_radius/2),.4f+1.25*sin(.35+j_radius/2),-5);
	glRotatef(-45,0,0,1);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[6]);
	j_Text[7].Use();
	glColor4ub(255,255,255,80);
	//j_drawquad(3.0f);
	}

	glLoadIdentity();
	glTranslatef(0,.05,-8);
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[12]);
	j_Text[13].Use();
	glRotatef(90,0,0,1);
	glScalef (.51,-10,1);
	glColor4ub(255,255,255,255);
	j_drawquad(1);

	glLoadIdentity();
	glTranslatef(-.035,-.645,-3);
	glDisable(GL_BLEND);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[14]);
	j_Text[15].Use();
	glRotatef(90,0,0,1);
	glScalef(1.2,-3.4,1);
	if (j_radius<3.9f) glColor4ub((unsigned char)(192-192*j_radius/4),(unsigned char)(192-192*j_radius/4),(unsigned char)(192-192*j_radius/4),255);
	else glColor4ub((unsigned char)(192-192*3.9f/4),(unsigned char)(192-192*3.9f/4),(unsigned char)(192-192*3.9f/4),255);
	j_drawquad(1);
	glEnable(GL_BLEND);

	if ((j_radius>.8f)&&(j_radius<1.115))
	{
	glLoadIdentity();
	glTranslatef(-2.5,.05,-5);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[9]);
	j_Text[10].Use();
	glColor4ub(255,255,255,(unsigned char)(160*sin((j_radius-.8f)*10)*sin((j_radius-.8f)*10)));
	j_drawquad(1.75);
	}

	//glDisable(GL_FOG);
	glLoadIdentity();
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[15]);
	j_Text[16].Use();
	glTranslatef(0,.5,-10);
	glRotatef(1.5,1,0,0);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	for (int i=0; i<j_num; i++)
	{
		glPushMatrix();
		glTranslatef(-1+parts[i].j_x/2.3,-.6-parts[i].j_y/10,0);
		glColor4ub(parts[i].r,parts[i].g,parts[i].b,(unsigned char)(j_max*j_max*parts[i].a*(.75+.25*sin(parts[i].phase+j_radius*(i)))));
		glRotatef(j_radius*200,0,0,1);
		j_drawquad(parts[i].rad/8);
		glPopMatrix();
	}

	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[13]);
	j_Text[14].Use();
	glTranslatef(-5,-1.2,-10);
	glRotatef(-7,0,1,0);
	for (int i=0; i<10; i++)
	{
		glPushMatrix();
		glTranslatef(i*.75,0,0);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glRotatef(-90,1,0,0);
		j_drawtrail(5,-90,.9f,255,128,96,255);
		glPopMatrix();
	}

	glTranslatef(6,.2,6);
	glRotatef(45,0,1,0);
	for (int i=0; i<5; i++)
	{
		glPushMatrix();
		glTranslatef(i*.25,.1,0);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glRotatef(-88,1,0,0);
		j_drawtrail(3,-88,1.5f,255,192,128,255);
		glPopMatrix();
	}

	glTranslatef(-2.5,.2,0);
	glRotatef(-55,0,1,0);
	for (int i=0; i<10; i++)
	{
		glPushMatrix();
		glTranslatef(i*.35,0,0);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glRotatef(-84,1,0,0);
		j_drawtrail(2,-84,1.5f,192,212,255,255);
		glPopMatrix();
	}

	glTranslatef(-1,-.27,0);
	glRotatef(63,0,1,0);
	for (int i=0; i<8; i++)
	{
		glPushMatrix();
		glTranslatef(i*.2,0,0);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glRotatef(-85,1,0,0);
		j_drawtrail(3,-85,2.5f,192,212,255,255);
		glPopMatrix();
	}

	glLoadIdentity();
	glTranslatef(-1.77,-.24,-5);
	j_Text[12].Use();
	glPushMatrix();
	glRotatef(30*sin(j_radius*10),0,0,1);
	glScalef(.2,1,1);
	glTranslatef(0,.5,0);
	glColor4ub(128,128,128,(unsigned char)(128*j_max));
	j_drawquad(1);
	glPopMatrix();

	glPushMatrix();
	glRotatef(20*sin(1+j_radius*15),0,0,1);
	glScalef(.2,1.2,1);
	glTranslatef(0,.5,0);
	glColor4ub(128,128,128,(unsigned char)(128*j_max*j_max));
	j_drawquad(1);
	glPopMatrix();

	glPushMatrix();
	glRotatef(20*sin(2+j_radius*25),0,0,1);
	glScalef(.2,1.5,1);
	glTranslatef(0,.5,0);
	glColor4ub(128,128,128,(unsigned char)(128*j_max*j_max*j_max));
	j_drawquad(1);
	glPopMatrix();

	glLoadIdentity();
	glTranslatef(-.1,-.075,-1);
	//glDisable(GL_TEXTURE_2D);
	j_Text[3].Use();
	//glPointSize(4);
	//glBegin(GL_POINTS);
	for (int i=0; i<j_num/5; i++)
	{
		GLfloat time;
		time=(j_time-parts[i].init)/250000.0f;

		GLfloat alpha=parts[i].a;
		if (parts[i].xp>.2f) alpha=parts[i].a*(1.0f-(parts[i].xp-.2f)*50.0f);
		if (parts[i].xp<-.23f) alpha=parts[i].a*(1.0f-(-parts[i].xp-.23f)*50.0f);
		alpha=alpha*j_max;
		if (alpha<0) alpha=0;
		if ((i%2)==0)
		{
			if (parts[i].xp>0.0f) glColor4ub((unsigned char)(128+127*parts[i].xp*4), (unsigned char)(128-128*parts[i].xp*4), (unsigned char)(128-128*parts[i].xp*4),(unsigned char)(alpha));
			else glColor4ub(128,128,128,(unsigned char)(alpha));
			//if (parts[i].xp>0.0f) glColor4ub(255*parts[i].xp*4,255-255*parts[i].xp*4,255-255*parts[i].xp*4,alpha);
			//else glColor4ub(255,255,255,alpha);
			//glVertex3f(parts[i].xp,.3*parts[i].xp*parts[i].xp,0);
			if (parts[i].xp>.22f)
			{
				parts[i].spd=.25+.0025*((GLfloat)(rand()%1000));
				parts[i].xp=-.25;
				parts[i].init=(long int)j_time;
			}
			else
			parts[i].xp=-.25f+parts[i].spd*time;
		}
		else
		{
			if (parts[i].xp<0.0f) glColor4ub((unsigned char)(128-127*parts[i].xp*4), (unsigned char)(128+128*parts[i].xp*4), (unsigned char)(128+128*parts[i].xp*4), (unsigned char)(alpha));
			else glColor4ub(128,128,128,(unsigned char)(alpha));
			//if (parts[i].xp<0.0f) glColor4ub(255-255*parts[i].xp*4,255*parts[i].xp*4,255*parts[i].xp*4,alpha);
			//else glColor4ub(255,255,255,alpha);
			//glVertex3f(parts[i].xp,.3*parts[i].xp*parts[i].xp,0);

			if (parts[i].xp<-.25f)
			{
				parts[i].spd=.25+.0025*((GLfloat)(rand()%1000));
				parts[i].xp=+0.22f;
				parts[i].init=(long int)j_time;

			}
			else
			parts[i].xp=.22f-parts[i].spd*time;
		}
		glPushMatrix();
		glTranslatef(parts[i].xp,.3*parts[i].xp*parts[i].xp,0);
		j_drawquad(.01);
		j_drawquad(.015);
		glPopMatrix();
	}
	//glEnd();

	//glEnable
	GLfloat credinit=-.6f;
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glColor4f(.75,.75,.75,1);
	j_Text[18].Use();
	glLoadIdentity();
	glScalef(1,-1,1);
	glTranslatef(1.1,0,-3.0f);
	j_drawcred(.45,1.25,credinit+j_radius/3.1f,.9f);
	j_Text[17].Use();
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, j_Text[ure[1]);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glColor4f(1,1,1,1);
	j_drawcred(.45,1.25,credinit+j_radius/3.1f,.9f);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_FOG);


	GLfloat lime=.75f;
	if (j_radius>lime)
	{
	if (j_FogColor[0]>0.2f) j_FogColor[0]=1.0f-(j_radius-lime);
	if (j_FogColor[1]>0.2f) j_FogColor[1]= .8f-(j_radius-lime)*.9f;
	if (j_FogColor[2]>0.2f) j_FogColor[2]= .5f-(j_radius-lime)*.5f;
	}
	glClearColor(.07f-j_radius/5.0f, 0.1f-j_radius/5.0f, 0.25f-j_radius/5.0f, 0.0f);
	if ((j_radius>.5f)&&(j_radius<1.5f)) j_max=j_radius-.5f;
}
	if (j_radius<.075)
	{
		GLfloat j_fader=.5f*(1.0f+cos(j_radius*3.1415f/.075f));
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,j_fader);
		j_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	if (j_radius>4.25f)
	{
		GLfloat j_fader=.5f*(1.0f-cos((j_radius-4.25f)*3.1415f/.5f));
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_ALPHA);
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,j_fader);
		j_drawquad(1.2f);
		glEnable(GL_TEXTURE_2D);
	}
	if (j_radius>4.75f)
	{
		//******************** FINISH
		//j_Clean();
		return false;
	}
/*	if ((j_radius>1.32)&&(th[0]))
	{
		th[0]=!th[0];
		FSOUND_PlaySound(FSOUND_FREE, th1);
		FSOUND_PlaySound(FSOUND_FREE, th1);
		FSOUND_PlaySound(FSOUND_FREE, th1);
	}
	if ((j_radius>1.48)&&(th[1]))
	{
		th[1]=!th[1];
		FSOUND_PlaySound(FSOUND_FREE, th2);
		FSOUND_PlaySound(FSOUND_FREE, th2);
		FSOUND_PlaySound(FSOUND_FREE, th2);
	}
*/
	glutSwapBuffers();
	return true;
}
