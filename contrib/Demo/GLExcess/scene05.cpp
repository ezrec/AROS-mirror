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
#include "scene05.h"
#define MAX_EMBOSS (GLfloat)0.01f

static Texture* c_Text;
static GLuint numtexs=15;
static GLuint width=800;
static GLuint height=600;
static bool init=true;
static bool c_first=true;

static GLfloat c_time=0;
static GLUquadricObj *c_quadratic;

static bool c_emboss=false;
static bool c_fader=false;

static GLfloat c_FogColor[]={ 1.0f, 1.0f, 1.0f, 1.0f };
static int c_x, c_y;

static int c_count=0;
static GLfloat	c_xrot;				// X Rotation ( NEW )
static GLfloat	c_yrot;				// Y Rotation ( NEW )
static GLfloat	c_zrot;				// Z Rotation ( NEW )
static GLfloat c_zeta=0.0f;
static GLfloat c_factor=1.0f;
static GLfloat c_maxshd=1.0f;

static int c_maxnum=0;
// static GLfloat c_Gray[]= {0.5f,0.5f,0.5f,1.0f};

static GLfloat c_shad=1.0f;
static GLfloat c_data[20]=
{
		// FRONT FACE
		0.0f, 0.0f,		-1.0f, -1.0f, +1.0f,
		1.0f, 0.0f,		+1.0f, -1.0f, +1.0f,
		1.0f, 1.0f,		+1.0f, +1.0f, +1.0f,
		0.0f, 1.0f,		-1.0f, +1.0f, +1.0f,
};

//static GLuint	c_text[1];
//static GLuint  c_bump[1];
//static GLuint  c_invbump[1];
static GLfloat c_radius=0.0f;

static const int c_num=50;
static const int c_numpart=10;
static int c_ci[c_numpart];

typedef struct
{
	GLfloat size;
	GLfloat phase;
	GLfloat freq;
	GLfloat amp;
	GLfloat spd;
	GLfloat c_y;
	bool twice;
	int r;
	int g;
	int b;
	int a;
}
c_part;

static c_part c_parts[2*c_num];
static c_part *c_fire[c_numpart];

static GLfloat c_LightAmbient[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
static GLfloat c_LightDiffuse[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
static GLfloat c_LightPosition[]=	{ 0.0f, 8.0f, -20.0f, 1.0f };

void c_initLights(void)
{
	glLightfv( GL_LIGHT1, GL_AMBIENT, c_LightAmbient);		// Load Light-Parameters into GL_LIGHT1
	glLightfv( GL_LIGHT1, GL_DIFFUSE, c_LightDiffuse);
	glLightfv( GL_LIGHT1, GL_POSITION, c_LightPosition);
	glEnable(GL_LIGHT1);
}

void c_InitGL()
{
	c_Text=new Texture[numtexs];
	c_maxnum=0;
	c_zeta=0.0f;
	c_factor=1.0f;
	c_maxshd=1.0f;
	c_radius=0.0f;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,30.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	c_Text[1].Load("data/star.raw");
	c_Text[2].Load("data/esaflr.raw");
	c_Text[3].Load("data/rusty3.raw");
	c_Text[4].Load("data/noise1.raw");
	c_Text[5].Load("data/lightmask.raw");
	c_Text[6].Load("data/text.raw");
	c_Text[7].Load("data/spot.raw");
	c_Text[8].Load("data/envmap.raw");
	c_Text[9].Load("data/sh1.raw");
	c_Text[10].Load("data/bump5.raw");
	c_Text[11].Load("data/floor1.raw");
	c_Text[12].Load("data/bumphalf.raw");
	c_Text[13].Load("data/mamor.raw");
	c_Text[14].Load("data/bumpinvhalf.raw");

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT1);
	for (int i=0; i<c_numpart; i++) c_fire[i]=new c_part[c_num];

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);


	glFogf(GL_FOG_MODE,GL_LINEAR);
	glFogf(GL_FOG_START,9.0f);
	glFogf(GL_FOG_END,28.0f);
	glFogf(GL_FOG_DENSITY,0.075f);
	c_FogColor[0]=0.0f;
	c_FogColor[1]=0.0f;
	c_FogColor[2]=0.0f;
	glFogfv(GL_FOG_COLOR,c_FogColor);
	glEnable(GL_FOG);

	glDisable (GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glDisable (GL_LIGHTING);
	glEnable(GL_BLEND);

	for (c_x=0; c_x<c_numpart; c_x++)
	{
		for (c_y=0; c_y<c_num; c_y++)
		{
			if((c_x==0)||(c_x==1))
			{
				c_parts[c_y+50*c_x].size=.001*((GLfloat)(rand()%1000));
				c_parts[c_y+50*c_x].phase=3.1415+.002*((GLfloat)(rand()%1000));
				c_parts[c_y+50*c_x].freq=.001*((GLfloat)(rand()%1000));
				c_parts[c_y+50*c_x].spd=.001*((GLfloat)(rand()%1000));
				c_parts[c_y+50*c_x].amp=.001*((GLfloat)(rand()%1000));
				c_parts[c_y+50*c_x].c_y=0.0f;
				c_parts[c_y+50*c_x].r=192+rand()%15;
				c_parts[c_y+50*c_x].g=192+rand()%15;
				c_parts[c_y+50*c_x].b=224+rand()%31;
				c_parts[c_y+50*c_x].a=192+rand()%63;
			}

			c_fire[c_x][c_y].size=.0005*((GLfloat)(rand()%1000));
			c_fire[c_x][c_y].freq=.001*((GLfloat)(rand()%1000));
			c_fire[c_x][c_y].spd=.001*((GLfloat)(rand()%1000));
			c_fire[c_x][c_y].amp=.001*((GLfloat)(rand()%1000));
			c_fire[c_x][c_y].c_y=0.0f;
			c_fire[c_x][c_y].r=128+rand()%128;
			c_fire[c_x][c_y].g=64+rand()%64;
			c_fire[c_x][c_y].b=32+rand()%32;
			c_fire[c_x][c_y].a=rand()%128;
			c_fire[c_x][c_y].twice=true;
		}
		c_ci[c_x]=0;
	}

	c_Text[1].Use();
	c_quadratic=gluNewQuadric();
	gluQuadricNormals(c_quadratic, GLU_SMOOTH);
	gluQuadricTexture(c_quadratic, GL_TRUE);
	c_initLights();

	c_parts[0].size=1.75f;
	c_parts[0].phase=3.1415f/.9f;
	c_parts[0].freq=-.5;
	c_parts[0].a=255;
	c_parts[0].spd=.25f;

	c_parts[1].size=1.75f;
	c_parts[1].phase=-3.1415f/.8f;
	c_parts[1].freq=-.5;
	c_parts[1].a=255;
	c_parts[1].spd=.25f;
}

void c_VMatMult(GLfloat *M, GLfloat *v)
{
	GLfloat res[3];
	res[0]=M[ 0]*v[0]+M[ 1]*v[1]+M[ 2]*v[2]+M[ 3]*v[3];
	res[1]=M[ 4]*v[0]+M[ 5]*v[1]+M[ 6]*v[2]+M[ 7]*v[3];
	res[2]=M[ 8]*v[0]+M[ 9]*v[1]+M[10]*v[2]+M[11]*v[3];;
	v[0]=res[0];
	v[1]=res[1];
	v[2]=res[2];
	v[3]=M[15];			// homogenous coordinate
}

void c_SetUpBumps(GLfloat *n, GLfloat *c_ci, GLfloat *l, GLfloat *s, GLfloat *t)
{
	GLfloat v[3];							// vertex from current position to light
	GLfloat lenQ;							// used to normalize

	// calculate v from current vector c_ci to lightposition and normalize v
	v[0]=l[0]-c_ci[0];
	v[1]=l[1]-c_ci[1];
	v[2]=l[2]-c_ci[2];
	lenQ=(GLfloat) sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[0]/=lenQ;		v[1]/=lenQ;		v[2]/=lenQ;
	// project v such that we get two values along each c_texture-coordinat axis.
	c_ci[0]=(s[0]*v[0]+s[1]*v[1]+s[2]*v[2])*MAX_EMBOSS;
	c_ci[1]=(t[0]*v[0]+t[1]*v[1]+t[2]*v[2])*MAX_EMBOSS;
}

void c_drawtrap(GLfloat top, GLfloat bot, GLfloat h)
{
	glPushMatrix();
	glTranslatef(0,-0.5f*h,0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.5f*bot,-0.5f*h,0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.5f*bot,-0.5f*h,0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.5f*top,0.5f*h,0.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-0.5f*top,0.5f*h,0.0f);
	glEnd();
	glPopMatrix();
}

void c_drawcyl1(int subdiv,GLfloat fact,GLfloat ratio,GLfloat angle)
{
	GLfloat a=1.0f;
	GLfloat b=1.0f;
	glBegin(GL_QUAD_STRIP);
	for (int i=0; i<=subdiv; i++)
	{
		glTexCoord2f(1.0f,ratio*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(fact,a*cos((angle/subdiv)*i*2*3.1415/360.0f),b*sin((angle/subdiv)*i*2*3.1415/360.0f));
		glTexCoord2f(0.0f,ratio*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(-fact,a*cos((angle/subdiv)*i*2*3.1415/360.0f),b*sin((angle/subdiv)*i*2*3.1415/360.0f));
	}
	glEnd();
}

void c_drawdisk(int subdiv,GLfloat fact,GLfloat ratio)
{
	GLfloat a=fact;
	GLfloat b=fact;
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f,0.5f);
	glVertex3f(0.0f,0.0f,0.0f);
	for (int i=0; i<=subdiv; i++)
	{
		glTexCoord2f(.5+.5*cos((360.0f/subdiv)*i*2*3.1415/360.0f),.5+.5*sin((360.0f/subdiv)*i*2*3.1415/360.0f));
		glVertex3f(a*cos((360.0f/subdiv)*i*2*3.1415/360.0f),b*sin((360.0f/subdiv)*i*2*3.1415/360.0f),0);
	}
	glEnd();
}

void c_drawquad(GLfloat size)
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

void c_drawquadm(GLfloat size, GLfloat tex, GLfloat tey)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f*tex, 0.0f);
		glVertex3f(0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f*tex, 1.0f*tey);
		glVertex3f(0.5f*size,0.5f*size,0.0f);
		glTexCoord2f(0.0f, 1.0f*tey);
		glVertex3f(-0.5f*size,0.5f*size,0.0f);
	glEnd();
}

void c_drawquadm0(GLfloat size, GLfloat tex, GLfloat tey, int cl,GLfloat off)
{
	glBegin(GL_QUADS);
		glColor4ub(cl,cl,cl,cl);
		glTexCoord2f(0.0f, 0.0f+off);
		glVertex3f(-0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f*tex, 0.0f+off);
		glVertex3f(0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f*tex, 1.0f*tey+off);
		glColor4ub(0,0,0,255);
		glVertex3f(0.5f*size,0.5f*size,0.0f);
		glTexCoord2f(0.0f, 1.0f*tey+off);
		glVertex3f(-0.5f*size,0.5f*size,0.0f);
	glEnd();
}

void c_drawquada(GLfloat size,GLfloat tex)
{
	glBegin(GL_QUADS);
		glColor4f(0.25f,0.25f,0.25f,1.0f);
		glTexCoord2f(0.0f, 0.0f-c_zeta);
		glVertex3f(-0.5f*size,-0.5f*size,0);
		glTexCoord2f(1.0f, 0.0f-c_zeta);
		glVertex3f(0.5f*size,-0.5f*size,0);
		glTexCoord2f(1.0f, 1.0f*tex-c_zeta);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		glVertex3f(0.5f*size,0.5f*size,0);
		glTexCoord2f(0.0f, 1.0f*tex-c_zeta);
		glVertex3f(-0.5f*size,0.5f*size,0);
	glEnd();
}

void c_drawquad0(int subdiv,GLfloat fact,GLfloat ratio)
{
	GLfloat a=3.0f;
	GLfloat b=1.75f;
	glBegin(GL_QUAD_STRIP);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(fact,a,-b);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-fact,a,-b);
	for (int i=0; i<=subdiv; i++)
	{
		glTexCoord2f(1.0f,.5f+.5f*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(fact,a*cos((75.0f/subdiv)*i*2*3.1415/360.0f),b*sin((75.0f/subdiv)*i*2*3.1415/360.0f));
		glTexCoord2f(0.0f,.5f+.5f*((GLfloat)i)/((GLfloat)subdiv));
		glVertex3f(-fact,a*cos((75.0f/subdiv)*i*2*3.1415/360.0f),b*sin((75.0f/subdiv)*i*2*3.1415/360.0f));
	}
	glEnd();
}

void c_drawcone(int sgn, GLfloat val)
{
	glEnable(GL_DEPTH_TEST);
	glColor4f(.2+val,.2+val,.2+val,1.0f);
	glPushMatrix();
	glScalef(.5,1,1);
	glTranslatef(sgn*.75,-.1,0);
	glRotatef(90,0,1,0);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	c_Text[8].Use();
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glRotatef(c_radius*8,0,0,1);
	glutSolidTorus(.1,.05,4,20);
	glPopMatrix();
	glColor4f(.5+val/2,.5+val/2,.5+val/2,1.0f);
	glPushMatrix();
	if (sgn==1) glTranslatef(.075,-.1,0); else
	glTranslatef(-.4,-.1,0);
	glRotatef(90,0,1,0);
	//glBindTexture(GL_TEXTURE_2D, c_texture[7]);
	gluCylinder(c_quadratic,.03,.03,.35,10,1);
	glPopMatrix();
	glPushMatrix();
	glRotatef(sgn*30,0,0,1);
	glRotatef(90,0,0,1);
	glRotatef(-90,0,1,0);
	glColor4ub(255,255,255,255);
	//glBindTexture(GL_TEXTURE_2D, c_texture[3]);
	c_Text[4].Use();
	glutSolidCone(.05,.5,4,4);
	glTranslatef(0,0,.07);
	gluCylinder(c_quadratic,.06,.05,.1,10,1);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
}

void c_Clean(void)
{
	c_Text[1].Kill();
	c_Text[2].Kill();
	c_Text[3].Kill();
	c_Text[4].Kill();
	c_Text[5].Kill();
	c_Text[6].Kill();
	c_Text[7].Kill();
	c_Text[8].Kill();
	c_Text[9].Kill();
	c_Text[10].Kill();
	c_Text[11].Kill();
	c_Text[12].Kill();
	c_Text[13].Kill();
	c_Text[14].Kill();
	delete [] c_Text;
	init=true;
}

bool c_DrawGLScene(GLfloat globtime)
{
	if (init){c_InitGL();init=false;}
	c_time=5*globtime;
	if (-c_radius<20.0f)
	{
		c_count++;
	}
	else if (c_first)
	{
		c_factor=800.0f/(GLfloat)c_count;
		c_first=false;
	}

	if (-c_radius<108.15f) c_zeta=.095f*(-c_radius-20.0f)+1.5f;
	else if (-c_radius<113.15f) c_zeta=9.575f+0.3f*(-cos(((-c_radius-108.15f)/5.0f)*3.1415f/2.0f+3.1415f/2.0f)+1.0f);

	if (-c_radius<10.0f)
	{
		c_xrot=-2.0f-7.0f*(cos((-c_radius/10.0f)*3.1415f)+1.0f);
		c_yrot=-10.0f;
		c_zrot=-3.0f;
		c_zeta=1.0f;
	}
	else
	if (-c_radius<15.0f)
	{
		c_zrot=-3.0f-.5f*(-cos(((-c_radius-10.0f)/5.0f)*3.1415f)+1.0f);
		c_yrot=-10.0f+3.5f*(-cos(((-c_radius-10.0f)/5.0f)*3.1415f)+1.0f);
		c_zeta=1.0f;
	}
	else
	if (-c_radius<20.0f)
	{
		c_zrot=-4.0f+2.0f*(-cos(((-c_radius-15.0f)/5.0f)*3.1415f)+1.0f);
		c_xrot=-2.0f+1.0f*(-cos(((-c_radius-15.0f)/5.0f)*3.1415f)+1.0f);
		c_zeta=1.0f+0.5f*(-cos(((-c_radius-15.0f)/5.0f)*3.1415f/2.0f)+1.0f);
	}
	else
	if (-c_radius<30.0f)
	{
		c_yrot=-3.0f+4.0f*(-cos(((-c_radius-20.0f)/5.0f)*3.1415f)+1.0f)-2.5*(-cos(((-c_radius-20.0f)/10.0f)*3.1415f)+1.0f);
		c_xrot=-1.0f+1.0f*(cos(((-c_radius-20.0f)/5.0f)*3.1415f));
	}
	else
	if (-c_radius<40.0f)
	{
		c_yrot=-8.0+3.0f*(-cos(((-c_radius-30.0f)/5.0f)*3.1415f)+1.0f)+4.0f*(-cos(((-c_radius-30.0f)/10.0f)*3.1415f)+1.0f);
		c_xrot=2.0-2.0f*(cos(((-c_radius-30.0f)/5.0f)*3.1415f));
	}
	else
	if (-c_radius<90.0f)
	{
		c_yrot=(cos((-c_radius-65.0f)*3.1415f/25.0f)+1.0f)*1.5f*(sin(((-c_radius-40.0f)/20.0f)*3.1415f))*(sin(((-c_radius-40.0f)/20.0f)*3.1415f));
		c_xrot=1.0f*(-cos(((-c_radius-40.0f)/12.5f)*3.1415f)+1.0f)-.5f*(-cos(((-c_radius-40.0f)/6.25f)*3.1415f)+1.0f);
	}

	if ((-c_radius>35.0f)&&(-c_radius<75.0f))
	{
		c_zrot=.5f*(-cos(((-c_radius-35.0f)/10.0f)*3.1415f)+1.0f)-1.0f*(-cos(((-c_radius-35.0f)/5.0f)*3.1415f)+1.0f);
	}

	if ((-c_radius>130.0f)&&(-c_radius<150.0f))
	{
		c_xrot=9.0f*(-cos((-c_radius-130.0f)*3.1415f/40.0f)+1.0f);
		c_zeta=10.1745f+.5f*(-cos((-c_radius-130.0f)*3.1415f/40.0f)+1.0f);
		c_fader=true;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_FOG);

	if (c_ci[9]>=2)
	{
		GLfloat c_ci[4]={0.0f,0.0f,0.0f,1.0f};
		GLfloat n[4]={0.0f,0.0f,0.0f,1.0f};
		GLfloat s[4]={0.0f,0.0f,0.0f,1.0f};
		GLfloat t[4]={0.0f,0.0f,0.0f,1.0f};
		GLfloat l[4];
		GLfloat Minv[16];

		// Build inverse Modelview Matrix c_first. This substitutes one Push/Pop with one glLoadIdentity();
		// Simply build it by doing all transformations negated and in reverse order.
		glColor4ub(255,255,255,255);
		glLoadIdentity();
		glRotatef(-c_yrot,0.0f,1.0f,0.0f);
		glRotatef(-c_xrot,1.0f,0.0f,0.0f);
		glTranslatef(0.0f,0.0,-.1);
		glGetFloatv(GL_MODELVIEW_MATRIX,Minv);

		glLoadIdentity();
		glTranslatef(0,-5+.015*sin(c_radius)+.015*sin(30*c_zeta),0);
		glRotatef(5*c_xrot,1,0,0);
		glRotatef(5*c_yrot,0,1,0);
		glRotatef(5*c_zrot+.5*sin(.5f*c_radius),0,0,1);
		glTranslatef(0.0f,4.5f,-88+8.22*c_zeta);
		glRotatef(-60,1,0,0);

		// Transform the Lightposition into object coordinates:
		l[0]=c_LightPosition[0];
		l[1]=c_LightPosition[1];
		l[2]=c_LightPosition[2];
		l[3]=1.0f;					// homogenous coordinate
		c_VMatMult(Minv,l);
		glEnable (GL_DEPTH_TEST);
	//	PASS#1: Use c_texture "c_bump"				No Blend				No Lighting				No offset c_texture-coordinates
		c_Text[12].Use();
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		c_drawdisk(30,1,0);

		glPushMatrix();
		glScalef(2,1.0,1);
		//glBindTexture(GL_TEXTURE_2D, c_texture[12]);
		c_Text[13].Use();
		glTranslatef(0,-.5,-.4);
		c_drawquadm(2,1,1);
		glPopMatrix();
		glPushMatrix();
		glScalef(2,1,1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		//glBindTexture(GL_TEXTURE_2D, c_texture[6]);
		c_Text[7].Use();
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTranslatef(0,-.5,-.4);
		c_drawquadm0(2,2,2,255,0);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_BLEND);
		glPopMatrix();

		glPushMatrix();
		//glBindTexture(GL_TEXTURE_2D, c_texture[12]);
		c_Text[13].Use();
		glTranslatef(0,-1.5,-.4);
		glRotatef(90,1,0,0);
		glPushMatrix();
		glScalef(4,.1,1);
		glTranslatef(0,-.5,0);
		glRotatef(180,1,0,0);
		c_drawquadm0(1,1,.05,255,.5);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_BLEND);
		//glBindTexture(GL_TEXTURE_2D, c_texture[6]);
		c_Text[7].Use();
		glColor4ub(255,255,255,250);
		c_drawquadm0(1,1,.05,192,.5);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glPopMatrix();

		glPushMatrix();
		//glBindTexture(GL_TEXTURE_2D, c_texture[12]);
		c_Text[13].Use();
		glTranslatef(0,-1.5,-.5);
		glRotatef(60,1,0,0);
		glPushMatrix();
		glScalef(4,1.25,1);
		glTranslatef(0,-.5,0);
		glColor4ub(128,128,128,255);
		glRotatef(180,1,0,0);
		c_drawquadm0(1,1,.5,255,0);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_BLEND);
		//glBindTexture(GL_TEXTURE_2D, c_texture[5]);
		c_Text[6].Use();
		c_drawquadm0(1,2,1,128,0);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_BLEND);
		glPopMatrix();
		glScalef(4,.2,1.5);
		glTranslatef(0,-5.5,0);
		for (int steps=0; steps<3; steps++)
		{
		glTranslatef(0,-.5,.5);
		glRotatef(90,1,0,0);
		glColor4ub(255,255,255,255);
		//glBindTexture(GL_TEXTURE_2D, c_texture[12]);	// HORIZ
		c_Text[13].Use();
		glPushMatrix();
		glRotatef(180,1,0,0);
		c_drawquadm0(1,1,.25,255,((GLfloat)steps)/4.0f);
		glPopMatrix();
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_BLEND);
		//glBindTexture(GL_TEXTURE_2D, c_texture[10]);
		c_Text[11].Use();
		glPushMatrix();
		glRotatef(180,1,0,0);
		c_drawquadm0(1,1,.25,160,0);
		glPopMatrix();
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_BLEND);
		glTranslatef(0,.5,.5);
		glRotatef(-90,1,0,0);
		glColor4ub(128,128,128,255);
		//glBindTexture(GL_TEXTURE_2D, c_texture[12]);	// VERT
		c_Text[13].Use();
		glPushMatrix();
		glRotatef(180,1,0,0);
		c_drawquadm0(1,1,.125,228,((GLfloat)steps)/4.0f);
		glPopMatrix();
		}
		glPopMatrix();

		glColor4ub(255,255,255,255);
		glDisable (GL_DEPTH_TEST);
	// PASS#2:	Use c_texture "c_invbump"				Blend GL_ONE to GL_ONE				No Lighting				offset c_texture coordinates


		c_Text[14].Use();
		glBlendFunc(GL_ONE,GL_ONE);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);

		glBegin(GL_QUADS);
			// Front Face
			n[0]=0.0f;		n[1]=0.0f;		n[2]=1.0f;
			s[0]=1.0f;		s[1]=0.0f;		s[2]=0.0f;
			t[0]=0.0f;		t[1]=1.0f;		t[2]=0.0f;
			for (int i=0; i<4; i++)
			{
				c_ci[0]=c_data[5*i+2];
				c_ci[1]=c_data[5*i+3];
				c_ci[2]=c_data[5*i+4];
				c_SetUpBumps(n,c_ci,l,s,t);
				glTexCoord2f(c_data[5*i]+c_ci[0], c_data[5*i+1]+c_ci[1]);
				glVertex3f(c_data[5*i+2], c_data[5*i+3],0);
			}
		glEnd();

	// PASS#3:	Use c_texture "Base"				Blend GL_DST_COLOR to GL_SRC_COLOR (multiplies by 2)				Lighting enabled				no offset c_texture-coordinates
		if (!c_emboss)
		{
			glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			c_Text[10].Use();
			glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
			c_drawdisk(30,1,0);
		}
		glEnable (GL_DEPTH_TEST);

		c_LightPosition[0]=2*cos(c_radius/3);
		c_LightPosition[1]=2*sin(2*c_radius/3);
		c_LightPosition[2]=.1;

		glPushMatrix();
		//glBindTexture(GL_TEXTURE_2D,c_texture[2]);
		c_Text[3].Use();
		glDisable(GL_BLEND);
		glTranslatef(0,0,-.2);
		glRotatef(90,0,1,0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable (GL_DEPTH_TEST);
		//glEnable(GL_DEPTH_TEST);
		glTranslatef(.35,0,0);
		glScalef(-1,1,1);
		glColor4ub(255,255,255,96);

		glPushMatrix();
		if (-c_radius<130.0f)
		{
			glRotatef(90,1,0,0);
			c_drawcyl1(30,.15,5,180.0);
		}
		else
		{
			glRotatef(90+(-c_radius-130.0)*3.5,1,0,0);
			c_drawcyl1(30,.15,5-(-c_radius-130.0)/35,180.0-(-c_radius-130.0)*7);
		}
		glPopMatrix();

		glRotatef(90,1,0,0);
		glScalef(-1,1,1);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glTranslatef(-.35,0,0);
		glColor4ub(160,160,160,255);

		c_drawcyl1(30,.2,5,180.0);

		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		glPopMatrix();

		if (c_zeta>9.5)
		{
		glPushMatrix();
		glTranslatef(c_LightPosition[0]/2,c_LightPosition[1]/2,0);
		//glBindTexture(GL_TEXTURE_2D,c_texture[4]);
		c_Text[5].Use();
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		glColor4f((c_zeta-9.5f)*2,(c_zeta-9.5f)*2,(c_zeta-9.5f)*2,(c_zeta-9.5f)*2);
		glDisable (GL_DEPTH_TEST);
		glRotatef(60-5*c_xrot,1,0,0);
		c_drawquad(7.6);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		//glBindTexture(GL_TEXTURE_2D,c_texture[0]);
		c_Text[1].Use();
		glColor4f(1.0f,1.0f,1.0f,(c_zeta-9.5f)*2);
		c_drawquad(.5);
		//////////////////////////////////////////////////////////////////
		glTranslatef(0,-.5,0);
		if ((c_LightPosition[0]<1.5)&&(c_LightPosition[0]>-1.5))
		{
			if (c_LightPosition[1]<-1.7)
			{
				if (c_shad<((float)220)/255) c_shad+=((float)30*c_factor)/255;
			}
			else
			{
				if (c_shad>((float)50)/255) c_shad-=((float)50*c_factor)/255; else c_shad=0.0f;
			}
		}
		else
		if (c_LightPosition[1]<.1)
		{
			if (c_shad<((float)220)/255) c_shad+=((float)30*c_factor)/255;
		}
		else
		{
			if (c_shad>((float)30)/255) c_shad-=((float)30*c_factor)/255; else c_shad=0.0f;
		}
		glColor4f(1.0f,1.0f,1.0f,(c_zeta-9.5f)*c_shad);
		c_drawquad(.35);
		glPopMatrix();

		glPushMatrix();
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);
		glLightfv( GL_LIGHT1, GL_POSITION, c_LightPosition);
		glPopMatrix();
		}
		glDisable(GL_LIGHTING);
	}

	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();
	glTranslatef(0,-5+.015*sin(30*c_zeta)+.015*sin(c_radius),0);
	glRotatef(5*c_xrot,1,0,0);
	glRotatef(5*c_yrot,0,1,0);
	glRotatef(5*c_zrot+.5*sin(.5f*c_radius),0,0,1);
/////////////////////////////////////////////////////////////////////////////////
	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, c_texture[10]);
	c_Text[11].Use();
	glTranslatef(0,1.5,-35);
	glScalef(6,3,40);
	glRotatef(90,1,0,0);
	glTranslatef(0,.5,0);
	glColor4ub(255,255,255,255);
	c_drawquada(1,5);
	glPopMatrix();

	glPushMatrix();
	//glTranslatef(0,0,-35);//c_zeta-7.5);
	glTranslatef(0,1.5,-81.07+c_zeta*8.22);
	glScalef(5.5,1,3);
	glRotatef(90,1,0,0);
	//glTranslatef(0,.5,0);
	c_Text[9].Use();
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	//glBlendFunc(GL_ONE,GL_ONE);
	glColor4f(.7,.7,.7,1);
	//glScalef(1.0/1.7,1.0/20,1.0/20);
	glRotatef(180,1,0,0);
	c_drawquad(1);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);



	for (int i=0; i<c_numpart; i++)			// GROUND
	{
	int sign1=1;
	for (int k=0; k<i; k++) sign1=-sign1;
	glLoadIdentity();
	glTranslatef(0,-5+.015*sin(30*c_zeta)+.015*sin(c_radius),0);
	glRotatef(5*c_xrot,1,0,0);
	glRotatef(5*c_yrot,0,1,0);
	glRotatef(5*c_zrot+.5*sin(.5f*c_radius),0,0,1);

	if ((i%2)==0) {if ((c_zeta*8.22/(30.0*c_ci[i]+30.0-3.0f*i))>.98f) c_ci[i]++;}
	else {if ((c_zeta*8.22/(30.0*c_ci[i]+30.0-3.0f*i))>1.07f) c_ci[i]++;}
	if (c_ci[i]<2) glTranslatef(sign1*2.0,5,-30+3*i-c_ci[i]*30+c_zeta*8.22);


	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	if (c_ci[i]<2)
	{
	glPushMatrix();				// TOP
	glScalef(-sign1,1,1);
	glTranslatef(2.6,.25,0);
	glRotatef(90,0,1,0);
	glRotatef(-90,1,0,0);
	int shade=0;
	for (int p=0; p<c_num; p++) if (c_fire [i][p].c_y <.25) shade++;
	glColor4ub(255,255,255,(unsigned char)(((int)((GLfloat)255)*((GLfloat)shade)/((GLfloat)c_num))));
	//glBindTexture(GL_TEXTURE_2D, c_texture[6]);
	c_Text[7].Use();
	c_drawquad0 (10,2.75,2.6);
	glPopMatrix();

	glPushMatrix();				// SIDE
	glPushMatrix();
	glTranslatef(sign1*.4,-.1,0);
	glRotatef(90,0,1,0);
	glColor4ub(255,255,255,96);
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glDisable(GL_DEPTH_TEST);
	//glBindTexture(GL_TEXTURE_2D, c_texture[8]);
	c_Text[9].Use();
	glRotatef(10.0f*((GLfloat)shade)/((GLfloat)c_num)*sin(c_radius/3),0,0,1);
	c_drawtrap(.35,.15,-.5+1.5*((GLfloat)shade)/((GLfloat)c_num));
	c_drawtrap(.05,0.2,2.0*((GLfloat)shade)/((GLfloat)c_num));
	glColor4ub(255,255,255,96);
	glRotatef(7.5f*((GLfloat)shade)/((GLfloat)c_num)*sin(c_radius/5),0,0,1);
	c_drawtrap(.35,.15,-.5+1.5*((GLfloat)shade)/((GLfloat)c_num));
	c_drawtrap(.05,.2,2.0*((GLfloat)shade)/((GLfloat)c_num));
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glDisable(GL_BLEND);
	c_drawcone(sign1,((GLfloat)shade)/((GLfloat)c_num));			// CONE


	glRotatef(-5*c_zrot,0,0,1);
	glRotatef(-5*c_yrot,0,1,0);
	glRotatef(-5*c_xrot,1,0,0);
	glColor4ub(255,255,255,128);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glRotatef(10*i+c_radius*5,0,0,1);
	//glBindTexture(GL_TEXTURE_2D, c_texture[1]);
	c_Text[2].Use();
	c_drawquad(-.25+2*((GLfloat)shade)/((GLfloat)c_num));
					///////////////////////////////
					///////////////////////////////
					///////////////////////////////
																	// BRIGHT
	glPopMatrix();
//	glPushMatrix();	///////////////////////////////
					///////////////////////////////
					///////////////////////////////
	//glBindTexture(GL_TEXTURE_2D, c_Text[ure[0]);
	c_Text[1].Use();
	for (int p=0; p<c_num; p++)
	{
		glPushMatrix();
		glTranslatef(.1*(c_fire[i][p].amp*sin(c_fire[i][p].freq*c_radius+c_fire[i][p].phase)),c_fire[i][p].c_y,0);
		glColor4ub(c_fire[i][p].r,c_fire[i][p].g,c_fire[i][p].b,c_fire[i][p].a);
		glRotatef(-5*c_zrot,0,0,1);
		glRotatef(-5*c_yrot,0,1,0);
		glRotatef(-5*c_xrot,1,0,0);

		c_drawquad(c_fire[i][p].size*1.2);
		c_fire[i][p].c_y+=c_fire[i][p].spd/80;

		if (c_fire[i][p].size<.2)//&&((p%4)!=0))
		{
			if (c_fire[i][p].twice) c_fire[i][p].a-=2;
			else c_fire[i][p].a-=5;
		}
		else
		{
			c_fire[i][p].a-=1;
		}
		if (c_fire[i][p].a<0)
		{
			if (c_fire[i][p].size<.2)
			{
			if (c_fire[i][p].twice==false)
			{
			c_fire[i][p].size=.0005*((GLfloat)(rand()%1000));
			c_fire[i][p].phase=3.1415+.001*((GLfloat)(rand()%1000));
			c_fire[i][p].freq=.001*((GLfloat)(rand()%1000));
			c_fire[i][p].spd=.0005*((GLfloat)(rand()%1000));
			c_fire[i][p].amp=.001*((GLfloat)(rand()%1000));
			c_fire[i][p].c_y=0.0f;
			c_fire[i][p].r=128+rand()%128;
			c_fire[i][p].g=64+rand()%64;
			c_fire[i][p].b=32+rand()%32;
			c_fire[i][p].a=rand()%255;
			c_fire[i][p].twice=true;
			}
			else
			{
			c_fire[i][p].a=128+rand()%128;
			c_fire[i][p].spd=c_fire[i][p].spd/2;
			c_fire[i][p].twice=false;
			}
			}
			else
			{
			c_fire[i][p].size=.0005*((GLfloat)(rand()%1000));
			c_fire[i][p].phase=3.1415+.001*((GLfloat)(rand()%1000));
			c_fire[i][p].freq=.001*((GLfloat)(rand()%1000));
			c_fire[i][p].spd=.0005*((GLfloat)(rand()%1000));
			c_fire[i][p].amp=.001*((GLfloat)(rand()%1000));
			c_fire[i][p].c_y=0.0f;
			c_fire[i][p].r=128+rand()%128;
			c_fire[i][p].g=64+rand()%64;
			c_fire[i][p].b=32+rand()%32;
			c_fire[i][p].a=rand()%255;
			c_fire[i][p].twice=true;
			}
		}

		glPopMatrix();
	}


	}




	else if(i<2)
	{
		int shade=0;
		int p;
	glScalef(2,2,2);
	glTranslatef(sign1*.75,3,-44+c_zeta*4.11);
	glEnable(GL_DEPTH_TEST);
	glPushMatrix();				// TOP
	glScalef(sign1,1,1);
	glTranslatef(0,-.25,-.4);
	for (p=0; p<c_num; p++) if (c_fire [i][p].c_y <.25) shade++;
	glColor4ub(255,255,255,(unsigned char)(((int)((GLfloat)255)*((GLfloat)shade)/((GLfloat)c_num))));
	//glBindTexture(GL_TEXTURE_2D, c_Text[ure[6]);
	c_Text[7].Use();
	c_drawquad(1);
	glPopMatrix();

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();				// SIDE

	glRotatef(sign1*90,0,1,0);

								// SHADOW
	glPushMatrix();

	glTranslatef(sign1*.4,-.1,0);
	glRotatef(90,0,1,0);
	glColor4ub(255,255,255,96);
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glDisable(GL_DEPTH_TEST);
	//glBindTexture(GL_TEXTURE_2D, c_Text[ure[8]);
	c_Text[9].Use();
	glRotatef(10.0f*((GLfloat)shade)/((GLfloat)c_num)*sin(2*i+c_radius/3),0,0,1);
	c_drawtrap(.35,.15,-.5+1.5*((GLfloat)shade)/((GLfloat)c_num));
	c_drawtrap(.05,.2,1.0*((GLfloat)shade)/((GLfloat)c_num));
	glColor4ub(255,255,255,96);
	glRotatef(7.5f*((GLfloat)shade)/((GLfloat)c_num)*sin(2*i+c_radius/5),0,0,1);
	c_drawtrap(.35,.15,-.5+1.5*((GLfloat)shade)/((GLfloat)c_num));
	c_drawtrap(.05,.2,1.0*((GLfloat)shade)/((GLfloat)c_num));
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glDisable(GL_BLEND);
	c_drawcone(sign1,((GLfloat)shade)/((GLfloat)c_num));			// CONE

	glRotatef(-5*c_zrot,0,0,1);
	glRotatef(-5*c_yrot,0,1,0);
	glRotatef(-5*c_xrot,1,0,0);
	glColor4ub(255,255,255,128);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glPushMatrix();
	glRotatef(10*i+sign1*c_radius*5,1,0,0);
	//glBindTexture(GL_TEXTURE_2D, c_Text[ure[1]);
	c_Text[2].Use();
	glRotatef(90,0,1,0);
	c_drawquad(-.25+2*((GLfloat)shade)/((GLfloat)c_num));
	glPopMatrix();
	glPopMatrix();

	//glBindTexture(GL_TEXTURE_2D, c_Text[ure[0]);
	c_Text[1].Use();
	for (p=0; p<c_num; p++)
	{
		glPushMatrix();
		glTranslatef(.1*(c_fire[i][p].amp*sin(c_fire[i][p].freq*c_radius+c_fire[i][p].phase)),c_fire[i][p].c_y,0);
		glColor4ub(c_fire[i][p].r,c_fire[i][p].g,c_fire[i][p].b,c_fire[i][p].a);
		glRotatef(-5*c_zrot,0,0,1);
		glRotatef(-5*c_yrot,0,1,0);
		glRotatef(-5*c_xrot,1,0,0);

		c_drawquad(c_fire[i][p].size*1.2);
		c_fire[i][p].c_y+=c_fire[i][p].spd/80;

		if (c_fire[i][p].size<.2)
		{
			if (c_fire[i][p].twice) c_fire[i][p].a-=2;
			else c_fire[i][p].a-=5;
		}
		else
		{
			c_fire[i][p].a-=1;
		}
		if (c_fire[i][p].a<0)
		{
			if (c_fire[i][p].size<.2)
			{
			if (c_fire[i][p].twice==false)
			{
			c_fire[i][p].size=.0005*((GLfloat)(rand()%1000));
			c_fire[i][p].phase=3.1415+.001*((GLfloat)(rand()%1000));
			c_fire[i][p].freq=.001*((GLfloat)(rand()%1000));
			c_fire[i][p].spd=.0005*((GLfloat)(rand()%1000));
			c_fire[i][p].amp=.001*((GLfloat)(rand()%1000));
			c_fire[i][p].c_y=0.0f;
			c_fire[i][p].r=128+rand()%128;
			c_fire[i][p].g=64+rand()%64;
			c_fire[i][p].b=32+rand()%32;
			c_fire[i][p].a=rand()%255;
			c_fire[i][p].twice=true;
			}
			else
			{
			c_fire[i][p].a=128+rand()%128;
			c_fire[i][p].spd=c_fire[i][p].spd/2;
			c_fire[i][p].twice=false;
			}
			}
			else
			{
			c_fire[i][p].size=.0005*((GLfloat)(rand()%1000));
			c_fire[i][p].phase=3.1415+.001*((GLfloat)(rand()%1000));
			c_fire[i][p].freq=.001*((GLfloat)(rand()%1000));
			c_fire[i][p].spd=.0005*((GLfloat)(rand()%1000));
			c_fire[i][p].amp=.001*((GLfloat)(rand()%1000));
			c_fire[i][p].c_y=0.0f;
			c_fire[i][p].r=128+rand()%128;
			c_fire[i][p].g=64+rand()%64;
			c_fire[i][p].b=32+rand()%32;
			c_fire[i][p].a=rand()%255;
			c_fire[i][p].twice=true;
			}
		}

		glPopMatrix();
	}
	}
	}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
	if(c_zeta<8)
	{
	glDisable(GL_DEPTH_TEST);
	//glBindTexture(GL_TEXTURE_2D, c_Text[ure[0]);
	c_Text[1].Use();
	if (c_maxnum<=97) c_maxnum=3*(int)(-c_radius-36.0f);
	if ((-c_radius>30.0f)&&(-c_radius<38.0f))
	{
		if (-c_radius<33.0f) c_maxnum=1; else c_maxnum=2;
	}
	for (int pp=0; pp<c_maxnum; pp++)
	{
		glLoadIdentity();
		glRotatef(5*c_yrot,0,1,0);
		glRotatef(5*c_xrot,1,0,0);
		glTranslatef((1-c_zeta/5.0f+c_parts[pp].amp)*cos(c_radius*2*c_parts[pp].freq+c_parts[pp].phase),
					 (1-c_zeta/5.0f+c_parts[pp].amp)*sin(c_radius*2*c_parts[pp].freq+c_parts[pp].phase),
					  c_parts[pp].c_y+c_zeta);
		//glColor4ub(c_parts[pp].r,c_parts[pp].g,c_parts[pp].b,c_parts[pp].a);

		if (c_parts[pp].a>=0) c_parts[pp].a=(int)(255.0f*((((GLfloat)c_parts[pp].a)/255.0f)-.01f*c_parts[pp].spd*c_factor));
		glColor4f(c_parts[pp].r/255.0f,c_parts[pp].g/255.0f,c_parts[pp].b/255.0f,c_parts[pp].a/255.0f);
		glRotatef(-5*c_xrot,1,0,0);
		glRotatef(-5*c_yrot,0,1,0);
		glRotatef(c_radius*75.0f*c_parts[pp].spd,0,0,1);
		if ((pp%3)==0) c_drawquad(.5*c_parts[pp].size);
		else if ((pp%3)==1) c_drawquad(.5*c_parts[pp].size+.25*c_parts[pp].size*sin(c_parts[pp].spd*c_radius*7.5f));
		else c_drawquad(.5*c_parts[pp].size+.25*c_parts[pp].size*sin(c_parts[pp].spd*c_radius*15.0f));
		if ((-c_radius>30.0f)&&(-c_radius<45.0f))
		{
			glPushMatrix();
			glRotatef(-c_radius*75.0f*c_parts[pp].spd,0,0,1);
			if (pp==1) c_drawquad(.25*c_parts[pp].size);
			else if (pp==0) c_drawquad(.25*c_parts[pp].size+.25*c_parts[pp].size*sin(c_parts[pp].spd*c_radius*7.5f));
			glPopMatrix();
		}
		c_parts[pp].c_y-=c_factor*c_parts[pp].spd/2.0f;
		if (c_parts[pp].a<0)//&&(c_zeta<6.0f))
		{
			c_parts[pp].size=.001*((GLfloat)(rand()%1000));
			//c_parts[pp].spd=.001*((GLfloat)(rand()%1000));
			c_parts[pp].phase=3.1415+.002*((GLfloat)(rand()%1000));
			c_parts[pp].freq=.001*((GLfloat)(rand()%1000));
			c_parts[pp].amp=.001*((GLfloat)(rand()%1000));
			c_parts[pp].r=192+rand()%15;
			c_parts[pp].g=192+rand()%15;
			c_parts[pp].b=224+rand()%31;
			c_parts[pp].a=192+rand()%63;
			c_parts[pp].c_y=0.0f;
		}
	}
	}
	else if (c_zeta<10)
	{
		for (int pp=0; pp<c_maxnum; pp++)
		{
			if (c_zeta>9.0f) c_maxshd=1.0f-(c_zeta-9.0f);
			glLoadIdentity();
			glRotatef(5*c_zrot,0,0,1);
			glRotatef(5*c_yrot,0,1,0);
			glRotatef(5*c_xrot,1,0,0);
			glTranslatef((1-c_zeta/5.0f+c_parts[pp].amp)*cos(c_radius*2*c_parts[pp].freq+c_parts[pp].phase),
						 (1-c_zeta/5.0f+c_parts[pp].amp)*sin(c_radius*2*c_parts[pp].freq+c_parts[pp].phase),
						  c_parts[pp].c_y+c_zeta);
			glColor4f(c_parts[pp].r/255.0f,c_parts[pp].g/255.0f,c_parts[pp].b/255.0f,c_maxshd*c_parts[pp].a/255.0f);
			if (c_parts[pp].a>0) c_drawquad(.5*c_parts[pp].size);
			c_parts[pp].c_y-=c_factor*c_parts[pp].spd/4;
		}
	}

	if ((c_zeta>5.0f)&&(c_zeta<10.0f))
	{
	//glBindTexture(GL_TEXTURE_2D, c_Text[ure[0]);
	c_Text[1].Use();
	glLoadIdentity();
	glRotatef(5*c_zrot,0,0,1);
	glRotatef(5*c_yrot,0,1,0);
	glRotatef(5*c_xrot,1,0,0);
	glTranslatef(0,0,-5);
	if (c_zeta<9) glColor4f(1.0f,1.0f,1.0f,(c_zeta-5.0f)/3.0f);
	else glColor4f(1.0f,1.0f,1.0f,1.0f-(c_zeta-9.0f)*2);
	for(int c_ci=0; c_ci<4; c_ci++)
	{
		glRotatef((c_ci+1)*c_radius,0,0,1);
		c_drawquad(((c_ci+1)*((c_zeta-5.0f)/10.0f+(c_zeta-5.0f)/10.0f*sin(c_radius)*sin(c_radius)))/2.0f);
	}
	}

	if (c_fader)
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(0,0,-.1f);
		glColor4f(1.0f,1.0f,1.0f,(-c_radius-140.0f)/3.0f);
		c_drawquad(1);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
	}


	if (((-c_radius>29.4)&&(-c_radius<31.4))||((-c_radius>108.5)&&(-c_radius<110.5)))
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(0,0,-.1f);
		if (-c_radius<31.4) glColor4f(1.0f,1.0f,1.0f,.5f-.5f*cos((-c_radius-29.4f)*3.1415f));
		else glColor4f(1.0f,1.0f,1.0f,.5f-.5f*cos((-c_radius-108.5)*3.1415f));
		c_drawquad(1);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
	}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

	c_radius=-.0015f*(c_time);

	if (-c_radius>143.0f)
	{
		//************************* FINISH
		//c_Clean();
		return false;
	}
	glutSwapBuffers();
	return true;
}
