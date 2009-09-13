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
#include "scene12.h"

static Texture* k_Text;
static GLuint numtexs=7;
static GLuint width=800;
static GLuint height=600;
static bool init=true;
static GLfloat k_time=0;

static GLfloat k_timer;

void k_InitGL()
{
	k_Text=new Texture[numtexs];
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	k_Text[1].Load("data/glxcess.raw");
	k_Text[2].Load("data/cl.raw");
	k_Text[3].Load("data/glxcesss.raw");
	k_Text[4].Load("data/crite.raw");
	k_Text[5].Load("data/lightmask.raw");
	k_Text[6].Load("data/cl2.raw");
	glShadeModel(GL_FLAT);
	glClearColor(.5f, 0.3f, 0.2f, 0.0f);
	glClearColor(.0f, .0f, .0f, 0.0f);
	glClearDepth(1.0f);
	glDisable(GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT, GL_FILL);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
}

void k_Clean(void)
{
	k_Text[1].Kill();
	k_Text[2].Kill();
	k_Text[3].Kill();
	k_Text[4].Kill();
	k_Text[5].Kill();
	k_Text[6].Kill();
	delete [] k_Text;
	init=true;
}

void k_drawrect(GLfloat b,GLfloat h)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-b/2,-h/2,0.0f);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(b/2,-h/2,0.0f);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(b/2,h/2,0.0f);
	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-b/2,h/2,0.0f);
	glEnd();
}

void k_drawrect1(GLfloat b,GLfloat h,GLfloat shifta,GLfloat shiftb)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f+shifta,0.0f+shiftb);
	glVertex3f(-b/2,-h/2,0.0f);
	glTexCoord2f(1.5f+shifta,0.0f+shiftb);
	glVertex3f(b/2,-h/2,0.0f);
	glTexCoord2f(1.5f+shifta,1.5f+shiftb);
	glVertex3f(b/2,h/2,0.0f);
	glTexCoord2f(0.0f+shifta,1.5f+shiftb);
	glVertex3f(-b/2,h/2,0.0f);
	glEnd();
}

bool k_DrawGLScene(GLfloat globtime)
{
	if (init) {k_InitGL();init=false;}
	k_time=10*globtime;

	glClear(GL_COLOR_BUFFER_BIT);
	k_timer=-1.0f+(k_time)/5000.0f;

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	k_Text[2].Use();
	glLoadIdentity();
	glTranslatef(0,0,-5.0f);
	k_timer+=3;
	glPushMatrix();
	//glColor4f(1,1,1,.15+.05*sin(k_timer/13.0f));
	glColor4f(1,1,1,.25+.25*sin(k_timer/13.0f));
	glRotatef(20*sin(k_timer*2.0),1,0,0);
	glRotatef(90*sin(k_timer),0,0,1);
	k_drawrect1(8.5,8.5,k_timer/10.0f,.25f-k_timer/5.0f);
	glPopMatrix();

	//k_Text[2].Use();
	glPushMatrix();
	glRotatef(k_timer*10.0,0,0,1);
	glTranslatef(0,0,2.0f*sin(k_timer/1.0f));
	//glColor4f(1,1,1,.1+.1*cos(k_timer/9.0f));
	glColor4f(1,1,1,.3+.3*cos(k_timer/9.0f));
	k_drawrect1(10,10,.5f-k_timer/7.5f,.75f+k_timer/2.5f);
	glPopMatrix();

	glPushMatrix();
	//k_Text[6].Use();
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	//glColor4f(.75,.75,.75,.5);
	glColor4f(1,1,1,1);
	glTranslatef(0,0,2.0f*sin(k_timer/1.0f));
	glRotatef(k_timer*20.0,0,0,1);
	glTranslatef(0,0,1+2.0f*sin(k_timer/2.0f)*sin(k_timer/1.0f));
	k_drawrect1(10,10,.35f-k_timer/10.0f,.1f+k_timer/25.0f);
	glPopMatrix();
	k_timer-=3;
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	k_Text[1].Use();
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
	glLoadIdentity();
	if (k_timer<2.501f) glTranslatef(0,0,-7.0f+3.0f*sin(k_timer*3.1415f*.5f/2.5f));
	else glTranslatef(0,0,-4.0f);

if(k_timer<2.5f)
{
	glPushMatrix();
	if (k_timer<=1.0f)
	{
		k_Text[3].Use();
		glColor4f(k_timer,k_timer,k_timer,1);
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		k_drawrect(4.2,1.7);
		k_Text[1].Use();
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glColor4f(1,1,1,k_timer);
		k_drawrect(3.5,1);
	}
	else
	if (k_timer<1.1f)
	{
		k_Text[3].Use();
		glColor4f(1.0f-10.0f*(k_timer-1.0f),1.0f-10.0f*(k_timer-1.0f),1.0f-10.0f*(k_timer-1.0f),1);
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		k_drawrect(4.2,1.7);
		k_Text[1].Use();
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glColor4f(1,1,1,20.0f*(k_timer-1.0f));
		glTranslatef(0,0,15000.0f*(k_timer-1.0f)*(k_timer-1.0f)*(k_timer-1.0f)*(k_timer-1.0f));
		k_drawrect(3.5,1);
	}
	else
	{
		GLfloat j_tras=(1.0f+sin((-k_timer+1.1f)*.5*3.1415f/1.4f));
		glTranslatef(0,0,1.5*j_tras);
		glColor4f(1,1,1,j_tras);
		k_drawrect(3.5,1);
	}
	glPopMatrix();
}

	if (k_timer>=1.0f)
	{
	if (k_timer<1.1f)
	{
		glColor4f(1,1,1,1.0f-10.0f*(k_timer-1.0f));
		k_drawrect(3.5,1);
	}
	else
	{
	glPushMatrix();
	if (k_timer<2.5f)
	{
		GLfloat j_tras=-sin((-k_timer+1.1f)*.5*3.1415f/1.4f);
		k_Text[1].Use();
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glColor4f(1,1,1,j_tras);
		k_drawrect(3.5,1);
	}
	else
	{
		k_Text[3].Use();
		glColor4f(2.0f*(k_timer-2.5f),2.0f*(k_timer-2.5f),2.0f*(k_timer-2.5f),1);
		glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
		k_drawrect(4.2,1.7);
		k_Text[1].Use();
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glColor4f(1,1,1,1);
		k_drawrect(3.5,1);
	}
	glPopMatrix();
	}
	}

	if (k_timer<0.0f)
	{
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1,1,1,-k_timer);
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		k_drawrect(1.2,1.2);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_TEXTURE_2D);
	}


	k_Text[5].Use();
	glLoadIdentity();
	glTranslatef(0,0,-1.0);
//	if (k_timer<2.501f) glTranslatef(0,0,-7.0f+3.0f*sin(k_timer*3.1415f*.5f/2.5f));
//	else glTranslatef(0,0,-4.0f);

	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glColor4f(1,1,1,1);
	k_drawrect(1.85,1.25);
	//k_drawrect(4.8,2.1);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	if (k_timer>2.5f)
	{

		glLoadIdentity();
		glTranslatef(.03,-.25,-1.0f);
		k_Text[4].Use();
		if (k_timer-2.5f<.75f) glColor4f(1,1,1,k_timer-2.5f);
		else glColor4f(1,1,1,.75f);
		glScalef(1,-1,1);
		k_drawrect(1,.03125);
	}

	if (k_timer>4.0f)
	{
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1,1,1,(k_timer-4.0f)/3.25f);
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		k_drawrect(1.2,1.2);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_TEXTURE_2D);
	}
	if (k_timer>7.25f)
	{
		//*************** FINISH
		//k_Clean();
		return false;
	}
	glutSwapBuffers();
	return true;
}
