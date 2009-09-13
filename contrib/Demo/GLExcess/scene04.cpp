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
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Texture.h"
#include "scene04.h"

static Texture* d_Text;
static GLuint numtexs=9;
static GLuint width=800;
static GLuint height=600;
static bool init=true;
static int d_time=0;

static int d_y;
static int d_timer1=0;
static int d_timer2=0;
static int d_timer3=0;
static int d_timer4=0;
static int d_timer5=0;
static int d_offset=0;

static int d_ct=0;

static void d_rst1(int);
static void d_rst2(int);
static void d_rst3(int);
static void d_rst4(int);
static void d_rst5(int);
static void d_rstoff(void);

static const int d_num1=20;
static const int d_num2=20;
static const int d_num3=20;
static const int d_num4=20;
static const int d_num5=75;

static const int d_repeat=11;

typedef struct
{
	GLfloat size;
	GLfloat phase;
	GLfloat mod;
	GLfloat axrot;
	GLfloat spd;
	GLfloat x,d_y;
	GLfloat fct;
	int r;
	int g;
	int b;
	int a;
}
d_part;

d_part *xp1[d_num1];
d_part *xp2[d_num2];
d_part *xp3[d_num3];
d_part *xp4[d_num4];
d_part *xp5[d_num5];
GLfloat d_radius[d_repeat];
bool d_sound[d_repeat];

GLfloat d_off[d_repeat];
void d_Clean(void)
{
	d_Text[1].Kill();
	d_Text[2].Kill();
	d_Text[3].Kill();
	d_Text[4].Kill();
	d_Text[5].Kill();
	d_Text[6].Kill();
	d_Text[7].Kill();
	d_Text[8].Kill();
	delete [] d_Text;
	init=true;
}

void d_InitGL()
{
	d_Text=new Texture[numtexs];
	d_time=0;
	d_timer1=0;
	d_timer2=0;
	d_timer3=0;
	d_timer4=0;
	d_timer5=0;
	d_offset=0;

	d_ct=0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,90.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	d_Text[1].Load("data/envmap1.raw");
	d_Text[2].Load("data/xp1.raw");
	d_Text[3].Load("data/xp2.raw");
	d_Text[4].Load("data/xp9.raw");
	d_Text[5].Load("data/xp4.raw");
	d_Text[6].Load("data/xp8.raw");
	d_Text[7].Load("data/logocol.raw");
	d_Text[8].Load("data/smoke.raw");

	glShadeModel(GL_FLAT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDisable(GL_DEPTH_TEST);

	for (int i=0; i<d_repeat; i++)
	{
		xp1[i]=new d_part[d_num1];
		xp2[i]=new d_part[d_num2];
		xp3[i]=new d_part[d_num3];
		xp4[i]=new d_part[d_num4];
		xp5[i]=new d_part[d_num5];

		d_rst1(i);
		d_rst2(i);
		d_rst3(i);
		d_rst4(i);
		d_rst5(i);
		d_sound[i]=true;
		d_radius[i]=-.5f;
	}
	d_rstoff();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glEnable (GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);
}

void d_drawquad(GLfloat size)
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

void d_drawtri(int i,int d_y,GLfloat size,GLfloat xrot,GLfloat yrot,GLfloat zrot)
{
	glRotatef(xp5[i][d_y].mod*xrot,1,0,0);
	glRotatef(xp5[i][d_y].mod*yrot,0,1,0);
	glRotatef(xp5[i][d_y].mod*zrot,0,0,1);
	glBegin(GL_TRIANGLES);
		glVertex3f(-0.5f*size,-0.5f*size,0.0f);
		glVertex3f(0.5f*size,-0.5f*size*((GLfloat)d_y/d_num5),0.0f);
		glVertex3f(0.5f*size*((GLfloat)d_y/d_num5),0.5f*size,0.0f);
	glEnd();
}

void d_xpls1(int i)
{
	d_Text[5].Use();
	if ((d_timer2>10)&&(d_timer1<d_num1)) d_timer1++;
	for (int d_y=0; d_y<d_timer1; d_y++)
	{


		// DRAW PART
		if (xp1[i][d_y].a>0)
		{
		glPushMatrix();
		glColor4ub(xp1[i][d_y].r,xp1[i][d_y].g,xp1[i][d_y].b,xp1[i][d_y].a);
		glRotatef(xp1[i][d_y].phase,0,0,1);
		glTranslatef(xp1[i][d_y].mod,0,0);
		glRotatef(xp1[i][d_y].axrot,0,0,1);
		d_drawquad(xp1[i][d_y].size);

		// UPDATE VARS

		xp1[i][d_y].mod+=xp1[i][d_y].spd/3.0f;
		xp1[i][d_y].size+=xp1[i][d_y].spd/2.0f;
		xp1[i][d_y].axrot=xp1[i][d_y].axrot+.125f;
		if (xp1[i][d_y].size>1.5f*xp1[i][d_y].fct) xp1[i][d_y].spd=xp1[i][d_y].fct*xp1[i][d_y].spd/1.1f;
		if (xp1[i][d_y].spd<0.0125f) xp1[i][d_y].spd=0.0125f;
		if (xp1[i][d_y].r>=130) xp1[i][d_y].r-=1+(int)(xp1[i][d_y].mod);
		if (xp1[i][d_y].g>=90) xp1[i][d_y].g-=2+2*(int)(xp1[i][d_y].mod);
		if (xp1[i][d_y].b>=24) xp1[i][d_y].b-=4+4*(int)(xp1[i][d_y].mod);
		xp1[i][d_y].a-=(int)(2.0f);

		if (xp1[i][d_y].a<1) xp1[i][d_y].a=0;

		glPopMatrix();
		}
	}
}

void d_xpls2(int i)
{
	//glBindTexture(GL_TEXTURE_2D, d_texture[2]);
	d_Text[3].Use();
	if (d_timer2<d_num2) d_timer2++;
	for (d_y=0; d_y<d_timer2; d_y++)
	{
		if (xp2[i][d_y].a>0)
		{
		glPushMatrix();

		// DRAW PART

		glColor4ub(xp2[i][d_y].r,xp2[i][d_y].g,xp2[i][d_y].b,xp2[i][d_y].a);
		glRotatef(xp2[i][d_y].phase+2.0*((GLfloat)d_y/d_num2),0,0,1);
		glTranslatef(xp2[i][d_y].mod,0,0);
		glRotatef(xp2[i][d_y].axrot*3,0,0,1);
		d_drawquad(xp2[i][d_y].size);

		// UPDATE VARS

		xp2[i][d_y].mod+=xp2[i][d_y].spd/5.0f;
		xp2[i][d_y].size+=xp2[i][d_y].spd*1.25;
		xp2[i][d_y].axrot=xp2[i][d_y].axrot+.125f;
		if (xp2[i][d_y].size>.75f*xp2[i][d_y].fct) xp2[i][d_y].spd=xp2[i][d_y].fct*xp2[i][d_y].spd/1.1f;
		if (xp2[i][d_y].spd<0.0125f) xp2[i][d_y].spd=0.0125f;
		if (xp2[i][d_y].r>=130) xp2[i][d_y].r-=1+(int)(xp2[i][d_y].mod);
		if (xp2[i][d_y].g>=90) xp2[i][d_y].g-=2+2*(int)(xp2[i][d_y].mod);
		if (xp2[i][d_y].b>=24) xp2[i][d_y].b-=4+4*(int)(xp2[i][d_y].mod);
		xp2[i][d_y].a-=(int)(2.0f);

		if (xp2[i][d_y].a<1) xp2[i][d_y].a=0;

		glPopMatrix();
		}
	}
}

void d_xpls3(int i)
{
	//glBindTexture(GL_TEXTURE_2D, d_texture[1]);
	d_Text[2].Use();
	if (d_timer3<d_num3) d_timer3++;
	for (d_y=0; d_y<d_timer3; d_y++)
	{
		if (xp3[i][d_y].a>0)
		{
		glPushMatrix();

		// DRAW PART

		glColor4ub(xp3[i][d_y].r,xp3[i][d_y].g,xp3[i][d_y].b,xp3[i][d_y].a);
		glRotatef(xp3[i][d_y].phase,0,0,1);
		glTranslatef(xp3[i][d_y].mod,0,0);
		glRotatef(xp3[i][d_y].axrot*5,0,0,1);
		d_drawquad(xp3[i][d_y].size);

		// UPDATE VARS

		xp3[i][d_y].mod+=xp3[i][d_y].spd/5.0f;
		xp3[i][d_y].size+=xp3[i][d_y].spd*1.2;
		xp3[i][d_y].axrot=xp3[i][d_y].axrot+.125f;
		if (xp3[i][d_y].size>.75f*xp3[i][d_y].fct) xp3[i][d_y].spd=xp3[i][d_y].fct*xp3[i][d_y].spd/1.1f;
		if (xp3[i][d_y].spd<0.0125f) xp3[i][d_y].spd=0.0125f;
		if (xp3[i][d_y].r>=96) xp3[i][d_y].r-=1+(int)(xp3[i][d_y].mod);
		if (xp3[i][d_y].g>=64) xp3[i][d_y].g-=2+2*(int)(xp3[i][d_y].mod);
		if (xp3[i][d_y].b>=16) xp3[i][d_y].b-=4+4*(int)(xp3[i][d_y].mod);
		if ((d_y%3)==0) xp3[i][d_y].a-=(int)(2.0f); else if ((d_y%3)==1) xp3[i][d_y].a-=(int)(3.0f); else xp3[i][d_y].a-=(int)(4.0f);

		if (xp3[i][d_y].a<1) xp3[i][d_y].a=0;

		glPopMatrix();
		}
	}
}

void d_xpls4(int i)
{
	d_Text[6].Use();
	if (d_timer4<d_num4) d_timer4++;
	for (d_y=0; d_y<d_timer4; d_y++)
	{
		if (xp4[i][d_y].a>0)
		{
		glPushMatrix();

		// DRAW PART

		glColor4ub(xp4[i][d_y].r,xp4[i][d_y].g,xp4[i][d_y].b,xp4[i][d_y].a);
		glRotatef(xp4[i][d_y].phase,0,0,1);
		glTranslatef(xp4[i][d_y].mod,0,0);
		if ((xp4[i][d_y].phase<270.0f)&&(xp4[i][d_y].phase>90.0f)) glRotatef(xp4[i][d_y].axrot*5+3.0*((GLfloat)d_y/d_num4),0,0,1);
		else glRotatef(-xp4[i][d_y].axrot*5+3.0*((GLfloat)d_y/d_num4),0,0,1);
		d_drawquad(xp4[i][d_y].size);

		// UPDATE VARS

		xp4[i][d_y].mod+=xp4[i][d_y].spd/.9f;
		xp4[i][d_y].size+=xp4[i][d_y].spd*1.5;
		xp4[i][d_y].axrot=xp4[i][d_y].axrot+.125f;
		if (xp4[i][d_y].size>.75f*xp4[i][d_y].fct) xp4[i][d_y].spd=xp4[i][d_y].fct*xp4[i][d_y].spd/1.5f;
		if (xp4[i][d_y].spd<0.0125f) xp4[i][d_y].spd=0.0125f;
		xp4[i][d_y].r=(int)(255*sin(1.5*xp4[i][d_y].phase)*sin(1.5*xp4[i][d_y].phase));
		xp4[i][d_y].g=xp4[i][d_y].r;
		xp4[i][d_y].b=xp4[i][d_y].r;
		xp4[i][d_y].a-=(int)(2.0f);
		xp4[i][d_y].phase+=.1*((GLfloat)d_y/d_num4);

		if (xp4[i][d_y].a<1) xp4[i][d_y].a=0;

		glPopMatrix();
		}
	}
}

void d_xpls5(int i)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glDisable(GL_BLEND);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glBindTexture(GL_TEXTURE_2D, d_texture[0]);
	d_Text[1].Use();
	glDisable(GL_CULL_FACE);
	//glDisable(GL_TEXTURE_2D);
	if (d_timer5<d_num5) d_timer5++;
	for (d_y=0; d_y<d_timer5; d_y++)
	{
		if (xp5[i][d_y].a>0)
		{
		glPushMatrix();

		// DRAW PART

		glColor4ub(255,224,208,xp5[i][d_y].a);//xp4[i][d_y].a);
		glRotatef(xp5[i][d_y].phase,0,0,1);
		glTranslatef(xp5[i][d_y].mod/2.0f,0,xp5[i][d_y].mod);
		if ((d_y%2)==0) d_drawtri(i,d_y,xp5[i][d_y].size,xp5[i][d_y].axrot,0,0); else
		if ((d_y%3)==0) d_drawtri(i,d_y,xp5[i][d_y].size,0,xp5[i][d_y].axrot,0); else
		d_drawtri(i,d_y,xp5[i][d_y].size,xp5[i][d_y].axrot,xp5[i][d_y].axrot,0);

		// UPDATE VARS

		xp5[i][d_y].mod+=xp5[i][d_y].spd/3.0f;
		xp5[i][d_y].spd-=2*(GLfloat)(d_y/d_num5);
		xp5[i][d_y].axrot=xp5[i][d_y].axrot/1.005;
		xp5[i][d_y].a-=(int)(2.0f);

		if (xp5[i][d_y].a<1) xp5[i][d_y].a=0;

		glPopMatrix();
		}
	}
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glPolygonMode(GL_FRONT, GL_FILL);
}

void d_rst1(int i)
{
	d_timer1=0;
	for (d_y=0; d_y<d_num1; d_y++)
	{
		xp1[i][d_y].size=0.0f;
		xp1[i][d_y].phase=.360*(GLfloat)(rand()%1000);
		xp1[i][d_y].mod=0.0f;
		xp1[i][d_y].axrot=0.0f;
		xp1[i][d_y].spd=.075+.00025*(GLfloat)(rand()%1000);
		xp1[i][d_y].fct=.5+.0005*(GLfloat)(rand()%1000);
		xp1[i][d_y].r=255;
		xp1[i][d_y].g=224;
		xp1[i][d_y].b=208;
		xp1[i][d_y].a=rand()%255;
	}
}

void d_rst2(int i)
{
	d_timer2=0;
	for (d_y=0; d_y<d_num2; d_y++)
	{
		xp2[i][d_y].size=0.0f;
		xp2[i][d_y].phase=.360*(GLfloat)(rand()%1000);
		xp2[i][d_y].mod=0.5f;
		xp2[i][d_y].axrot=0.0f;
		xp2[i][d_y].spd=.025+.00025*(GLfloat)(rand()%1000);
		xp2[i][d_y].fct=1.025f;
		xp2[i][d_y].r=255;
		xp2[i][d_y].g=224;
		xp2[i][d_y].b=208;
		xp2[i][d_y].a=128+rand()%127;
	}
}

void d_rst3(int i)
{
	d_timer3=0;
	for (d_y=0; d_y<d_num3; d_y++)
	{
		xp3[i][d_y].size=0.0f;
		xp3[i][d_y].phase=.360*(GLfloat)(rand()%1000);
		xp3[i][d_y].mod=0.5f;
		xp3[i][d_y].axrot=0.0f;
		xp3[i][d_y].spd=.025+.00025*(GLfloat)(rand()%1000);
		xp3[i][d_y].fct=1.0f;
		xp3[i][d_y].r=255;
		xp3[i][d_y].g=224;
		xp3[i][d_y].b=208;
		xp3[i][d_y].a=128+rand()%127;
	}
}

void d_rst4(int i)
{
	d_timer4=0;
	for (d_y=0; d_y<d_num4; d_y++)
	{
		xp4[i][d_y].size=0.0f;
		xp4[i][d_y].phase=.360*(GLfloat)(rand()%1000);
		xp4[i][d_y].mod=0.5f;
		xp4[i][d_y].axrot=0.0f;
		xp4[i][d_y].spd=.025+.00025*(GLfloat)(rand()%1000);
		xp4[i][d_y].fct=1.0f;
		xp4[i][d_y].r=255;
		xp4[i][d_y].g=255;
		xp4[i][d_y].b=255;
		xp4[i][d_y].a=192+rand()%63;
	}
}

void d_rst5(int i)
{
	d_timer5=0;
	for (d_y=0; d_y<d_num5; d_y++)
	{
		xp5[i][d_y].size=.1f+.00025f*((GLfloat)(rand()%1000));
		xp5[i][d_y].phase=.360*(GLfloat)(rand()%1000);
		xp5[i][d_y].mod=0.0f;
		xp5[i][d_y].axrot=.5f*((GLfloat)(rand()%1000));
		xp5[i][d_y].spd=.05+.0005*(GLfloat)(rand()%1000);
		xp5[i][d_y].a=128+rand()%127;
	}
}

void d_rstoff(void)
{
	for (int i=0; i<d_repeat; i++)
	d_off[i]=.5f+.0005f*((GLfloat)(rand()%1000));
	d_off[0]=.7;
	d_off[1]=.8;
	d_off[2]=.7;
	d_off[3]=.85;
	d_off[4]=.7;
	d_off[5]=.9;
	d_off[6]=.8;
	d_off[7]=.7;
	d_off[8]=.9;
	d_off[9]=.8;
	d_off[10]=1.2;
}

bool d_DrawGLScene()
{
	if (init) {d_InitGL();init=false;}
	d_time++;
	d_ct++;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0,0,-15);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0,0,-15);
	glScalef(2,1,1);
	d_Text[7].Use();
	glDisable(GL_BLEND);

	if (d_ct<300) glColor4ub(255,255,255,225);
	else glColor4ub((unsigned char)(255-(GLfloat)2.55f*((GLfloat)(d_ct-300))),(unsigned char)(255-(GLfloat)2.55f*((GLfloat)(d_ct-300))),(unsigned char)(255-(GLfloat)2.55f*((GLfloat)(d_ct-300))),225);
	d_drawquad(5);
	glEnable(GL_BLEND);
	glPopMatrix();
	if ((d_offset<=d_repeat-1)&&(d_time>20))
	{
		d_offset++;
		d_time=0;
	}
	for (int i=0; i<d_offset; i++)
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glPushMatrix();
		if (i!=d_repeat-1)
		glTranslatef(4*cos(6.28f*((GLfloat)i/(d_repeat-1))),2*sin(6.28f*((GLfloat)i/(d_repeat-1))),0);
		//else MessageBox(NULL,"i vale repeat-1","i vale repeat-1",0);
		glScalef(d_off[i],d_off[i],1);
		glColor4ub(255,255,255,255);
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		//glBindTexture(GL_TEXTURE_2D, d_texture[7]);
		d_Text[8].Use();
		glPushMatrix();
		glRotatef(4*d_radius[i],0,0,1);
		d_drawquad(d_radius[i]/4);
		glPopMatrix();
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		//glBindTexture(GL_TEXTURE_2D, d_texture[3]);
		d_Text[4].Use();
		if ((1.0f-(d_radius[i]/2.5f))>0.0f)
		{
			glColor4f(.95f,.9f,.75f,1.0f-(d_radius[i]/2.5f));
			d_drawquad(4.0f+d_radius[i]*5.0f);
		}
		glRotatef(d_radius[i]*20,0,0,1);
		if ((1.0f-(d_radius[i]/2.5f))/2.0f>0.0f)
		{
			glColor4f(.95f,.9f,.75f,(1.0f-(d_radius[i]/2.5f))/2.0f);
			d_drawquad(4.0f+d_radius[i]*8.0f);
		}
		glPopMatrix();
		if (d_radius[i]>0.0f)
		{
		/*	if (d_sound[i])
			{
				if (i==d_repeat-1)
				{
					FSOUND_PlaySound(FSOUND_FREE,xp02);
					//FSOUND_PlaySound(FSOUND_FREE,xp02);
					FSOUND_PlaySound(FSOUND_FREE,xp03);
					//FSOUND_PlaySound(FSOUND_FREE,xp04);
					FSOUND_PlaySound(FSOUND_FREE,xp05);
					FSOUND_PlaySound(FSOUND_FREE,xp05);
				}
				else
				for (int ax=0; ax<1; ax++)
				{
				GLfloat rundmc=((GLfloat)(rand()%1000))*.001;
				if (rundmc<.25f) FSOUND_PlaySound(FSOUND_FREE, xp01);
				else if (rundmc<.5f) FSOUND_PlaySound(FSOUND_FREE, xp02);
				else if (rundmc<.75f) FSOUND_PlaySound(FSOUND_FREE, xp03);
				else FSOUND_PlaySound(FSOUND_FREE, xp04);
				}
				//if ((i==1)||(i==5)||(i==9))
				if (((i%3)==0)||(i==d_repeat-1))
				FSOUND_PlaySound(FSOUND_FREE,crash);

				d_sound[i]=false;
			}*/
		glPushMatrix();
		if (i!=d_repeat-1)
		glTranslatef(4*cos(6.28f*((GLfloat)i/(d_repeat-1))),2*sin(6.28f*((GLfloat)i/(d_repeat-1))),0);
		glScalef(d_off[i],d_off[i],1);
		//glDisable(GL_TEXTURE_2D);
		d_xpls1(i);

		d_xpls2(i);

		d_xpls3(i);

		d_xpls4(i);

		d_xpls5(i);

		glPopMatrix();
		}
		d_radius[i]+=.25f;
	}
	if (d_ct>400)
	{
		//****************************************** FINISH
	//	d_Clean();
		return false;
	}
#ifdef WIN32
	Sleep(20);
#else
	usleep(20);
#endif
	glutSwapBuffers();
	return true;
}
