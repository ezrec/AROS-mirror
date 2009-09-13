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
#include "scene06.h"
#include "scene06-data.h"

static Texture* e_Text;
static GLuint numtexs=10;
static GLuint width=800;
static GLuint height=600;
static bool init=true;
static GLfloat e_time=0;

static bool dum[4];
static bool e_lasers=false;
static bool e_scene=false;

static GLfloat e_FogColor[]={ 1.0f, 1.0f, 1.0f, 1.0f };

static GLfloat	e_xrot;
static GLfloat	e_yrot;
static GLfloat	e_zrot;
static GLfloat e_timer=-1.25f;
static GLfloat e_speed=0.0f;

static GLfloat e_zeta=0.0f;
static GLfloat e_fade=0.0f;

static GLfloat e_rocca=1.0f;
static GLfloat e_depth=60.0f;

static GLfloat e_radius=0.0f;
static GLuint shiplist;

GLint genship()
{
	int i,j;
	GLint lid=glGenLists(1);
	glNewList(lid, GL_COMPILE);

	glBegin (GL_TRIANGLES);
	for(i=0;i<(int)(sizeof(face_indicies)/sizeof(face_indicies[0]));i++)
	{if ((i%1)==0)
		{
			for(j=0;j<3;j++)
			{
				int vi=face_indicies[i][j];
				int ni=face_indicies[i][j+3];//Normal index
				glNormal3f (normals[ni][0],normals[ni][1],normals[ni][2]);
				glVertex3f (vertices[vi][0],vertices[vi][1],vertices[vi][2]);
			}
		}
	}
	glEnd ();
	glEndList();
	return lid;
};

void  e_InitGL()
{
	e_Text=new Texture[numtexs];
	shiplist=genship();
	for (int ev=0; ev<4; ev++) dum [ev]=true;
	e_lasers=false;
	e_scene=false;
	e_xrot=0;e_yrot=0;e_zrot=0;
	e_timer=-1.25f;e_speed=0.0f;e_zeta=0.0f;e_fade=0.0f;
	e_rocca=1.0f;e_depth=60.0f;
	e_radius=0.0f;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,90.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	e_Text[1].Load("data/star.raw");
	e_Text[2].Load("data/cl.raw");
	e_Text[3].Load("data/mtop.raw");
	e_Text[4].Load("data/text2.raw");
	e_Text[5].Load("data/metal.raw");
	e_Text[6].Load("data/rusty2.raw");
	e_Text[7].Load("data/mfloor1.raw");
	e_Text[8].Load("data/ship.raw");
	e_Text[9].Load("data/sground.raw");

	glShadeModel(GL_SMOOTH);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	//glFogf(GL_FOG_MODE,GL_LINEAR);
	glFogf(GL_FOG_MODE,GL_EXP2);
	glFogf(GL_FOG_START,15.0f);
	glFogf(GL_FOG_END,25.0f);
	//glFogf(GL_FOG_DENSITY,0.175f);
	glFogf(GL_FOG_DENSITY,0.075f);
	e_FogColor[0]=0.0f;
	e_FogColor[1]=0.0f;
	e_FogColor[2]=0.0f;
	glFogfv(GL_FOG_COLOR,e_FogColor);
	glEnable(GL_FOG);
	glDisable (GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_FOG);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	e_Text[1].Use();
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
}

void e_Clean(void)
{
	e_Text[1].Kill();
	e_Text[2].Kill();
	e_Text[3].Kill();
	e_Text[4].Kill();
	e_Text[5].Kill();
	e_Text[6].Kill();
	e_Text[7].Kill();
	e_Text[8].Kill();
	e_Text[9].Kill();
	delete [] e_Text;
	init=true;
}

void e_drawquad(GLfloat size)
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

void e_drawmquad(GLfloat size, GLfloat mtex)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f-.1*e_zeta*e_rocca);
		glVertex3f(-0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f, 0.0f-.1*e_zeta*e_rocca);
		glVertex3f(0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f, 1.0f*mtex-.1*e_zeta*e_rocca);
		glVertex3f(0.5f*size,0.5f*size,0.0f);
		glTexCoord2f(0.0f, 1.0f*mtex-.1*e_zeta*e_rocca);
		glVertex3f(-0.5f*size,0.5f*size,0.0f);
	glEnd();
}

void e_drawmquad1(GLfloat size, GLfloat mtex)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f+.1*e_zeta*e_rocca, 0.0f);
		glVertex3f(-0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f*mtex+.1*e_zeta*e_rocca, 0.0f);
		glVertex3f(0.5f*size,-0.5f*size,0.0f);
		glTexCoord2f(1.0f*mtex+.1*e_zeta*e_rocca, 1.0f);
		glVertex3f(0.5f*size,0.5f*size,0.0f);
		glTexCoord2f(0.0f+.1*e_zeta*e_rocca, 1.0f);
		glVertex3f(-0.5f*size,0.5f*size,0.0f);
	glEnd();
}

void e_drawtrail(GLfloat tsz)
{
	//glBindTexture(GL_TEXTURE_2D, e_texture[0]);
	e_Text[1].Use();
	glScalef(1/.75,1/.25,1/.05);

	for (int zx=0; zx<4; zx++)
	{
		glPushMatrix();
		glTranslatef(-.3+(GLfloat)zx/5,0,0);
		glRotatef(-5*e_yrot,0,1,0);
		glRotatef(-90,0,1,0);
		glRotatef(-45-e_yrot*2,0,0,1);
		e_drawquad(tsz);
		glPopMatrix();
	}
}

void e_drawtrailup(GLfloat tsz)
{
	//glBindTexture(GL_TEXTURE_2D, e_texture[0]);
	e_Text[1].Use();
	glScalef(1/.25,1/.75,1/.05);

	for (int zx=0; zx<4; zx++)
	{
		glPushMatrix();
		glTranslatef(0,-.3+(GLfloat)zx/5,0);
		glRotatef(-5*e_xrot,1,0,0);
		glRotatef(-90,1,0,0);
		glRotatef(-45-e_xrot*2,0,0,1);
		e_drawquad(tsz);
		glPopMatrix();
	}
}

void e_Clear(GLfloat quad)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	glTranslatef(0,0,-1.0f);
	glColor4f(0,0,0,1-quad);
	e_drawquad(1.2f);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_TEXTURE_2D);
}

bool e_DrawGLScene(GLfloat globtime)
{
	if (init) {e_InitGL();init=false;}
	e_time=3*globtime;
	GLfloat eoffset=4.75;
	//glClearColor(e_FogColor[0]+.005,e_FogColor[0]+.005,e_FogColor[0]+.005,1);
	if ((e_timer<eoffset)||(e_timer>eoffset+2.0)) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else {glClear(GL_DEPTH_BUFFER_BIT);	e_Clear(.5f-.5*cos((e_timer-eoffset)*3.1415));}


	e_timer=-2.0f+(e_time)/3300.0f;
	if (e_scene)
	{
	glDisable(GL_DEPTH_TEST);
	glLoadIdentity();
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);
	glRotatef(5*e_zrot,0,0,1);
	glRotatef(5*e_yrot,0,1,0);
	glRotatef(5*e_xrot,1,0,0);
	glTranslatef(0,0,-40);
	glColor4f(e_FogColor[0],e_FogColor[0],e_FogColor[0],1);
	//glColor4f(1,1,1,1);
	if ((e_timer<1)||(e_timer>13)) e_drawquad(10);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_FOG);



	e_depth=30+30*e_radius;

	if (e_timer>-1.0f)
	{
	if ((e_timer<eoffset)||(e_timer>eoffset+2)) glDisable(GL_BLEND); else glEnable(GL_BLEND);
	glLoadIdentity();

	glTranslatef(.75f*sin(e_timer),.35f*cos(e_timer),0);

	glRotatef(5*e_zrot,0,0,1);
	glRotatef(5*e_yrot,0,1,0);
	glRotatef(5*e_xrot,1,0,0);

	if ((e_timer>6.0f)&&(e_timer<10.0)) e_xrot=(6.0f-((e_timer-6.0f)*1.5f))*(cos((e_timer-6.0f)*3.1415f/2.0f)-1.0f);
	if ((e_timer>5.0f)&&(e_timer<9.0)) e_yrot=(36.0f-((e_timer-5.0f)*9.0f))*(cos((e_timer-5.0f)*3.1415f/2.0f)-1.0f);
	if ((e_timer>2.0f)&&(e_timer<6.0)) e_zrot=-(9.0f-((e_timer-2.0f)*4.5f))*(cos((e_timer-2.0f)*3.1415f/2.0f)-1.0f);

	if ((e_timer>10.0f)&&(e_timer<12.0)) e_xrot=-(6.0f-((e_timer-10.0f)*3.0f))*(cos((e_timer-10.0f)*3.1415f)-1.0f);
	if ((e_timer>12.0f)&&(e_timer<14.0)) e_xrot=(6.0f-((e_timer-12.0f)*3.0f))*(cos((e_timer-12.0f)*3.1415f)-1.0f);
	if ((e_timer>9.0f)&&(e_timer<13.0)) e_yrot=-(6.0f-((e_timer-9.0f)*1.5f))*(cos((e_timer-9.0f)*3.1415f/2.0f)-1.0f);
	if ((e_timer>6.0f)&&(e_timer<10.0)) e_zrot=-(16.0f-((e_timer-6.0f)*4.0f))*(cos((e_timer-6.0f)*3.1415f/2.0f)-1.0f);
	if ((e_timer>10.0f)&&(e_timer<16.0)) e_zrot=(6.0f-((e_timer-10.0f)*1.0f))*(cos((e_timer-10.0f)*3.1415f/3.0f)-1.0f);
	if ((e_timer>16.0f)&&(e_timer<20.0)) e_zrot=(9.0f-((e_timer-16.0f)*4.5f))*(cos((e_timer-16.0f)*3.1415f/3.0f)-1.0f);

	if ((e_timer>13.5f)&&(e_timer<15.0f)) e_radius=(cos((e_timer-13.5f)*3.1415f/1.5f+3.1415f)+1.0f)/2.0f;

	if ((e_timer>13.25f)&&(e_timer<19.25f)) e_speed=.125+(cos(3.1415f+(e_timer-13.25f)*3.1415f/4.0f)+1.0f)/50.0f;//e_speed=.125+sin(3.1415f*.5f*(e_timer-13.25))*.0125f;//e_speed=.125+(cos(3.1415f+(e_timer-13.25f)*3.1415f/2.0f)+1.0f)/50.0f;

	if (e_timer>16.0f) e_fade=(cos((e_timer-16.0f)*3.1415f/2.0f+3.1415f)+1.0f)*20.0f;

	if ((e_timer>15.0f)&&(dum[0]))
	{
		dum[0]=false; e_lasers=true;
	//FSOUND_PlaySound(FSOUND_FREE, blam);
	}

	int lim=2;
	if (e_timer>15.0f) lim=1;

	for(int fb=0; fb<lim; fb++)
	{
	glTranslatef(0,0,e_fade-e_depth/2+fb*3*e_depth/2);

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, e_Text[ure[2]);
	e_Text[3].Use();
	glScalef(1.122,1,e_depth);
	glTranslatef(0,2.591,0);
	glRotatef(90,1,0,0);
	if (e_timer<1.0f) glColor4f(e_timer/2.0f,e_timer/2.0f,e_timer/2.0f,e_timer/2.0f);
	else glColor4f(.5f,.5f,.5f,.5f);
	e_drawmquad(1,6);				// ROOF TOP
	glPopMatrix();

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, e_Text[ure[12]);
	e_Text[7].Use();
	glScalef(2,1,e_depth);
	glTranslatef(0,-1,0);
	glRotatef(90,1,0,0);
	if (e_timer<1.0f) glColor4f(e_timer,e_timer,e_timer,e_timer);
	else glColor4f(1.0f,1.0f,1.0f,1.0f);
	e_drawmquad(1,6);				// FLOOR
	glPopMatrix();

	for(int pp=0; pp<2; pp++)
	{
	glPushMatrix();
	if (pp==1) glScalef(-1,1,1);
	//glBindTexture(GL_TEXTURE_2D, e_Text[ure[11]);
	e_Text[6].Use();
	glScalef(1,1,e_depth);
	glTranslatef(-1.35,-.65,0);
	glRotatef(45,0,0,1);
	glRotatef(90,0,1,0);
	e_drawmquad1(1,3);				// BOTT LEFT
	glPopMatrix();

	glPushMatrix();
	if (pp==1) glScalef(-1,1,1);
	//glBindTexture(GL_TEXTURE_2D, e_Text[ure[9]);
	e_Text[5].Use();
	glScalef(1,3,e_depth);
	glTranslatef(-1.13,.383,0);
	glRotatef(-50,0,0,1);
	glRotatef(90,0,1,0);
	glScalef(1,1.5,1);
	e_drawmquad1(1,6);				// TOP LEFT
	glPopMatrix();
	}
	}

	glTranslatef(0,0,e_zeta/1.75);
	//glColor4ub(255,255,255,255);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	if ((e_timer>1.0f)&&(e_timer<15.0f))
	{
		GLfloat var=0;
		if ((e_timer>3)&&(e_timer<3.4)) var=.5f-.5f*cos((e_timer-3)*3.1415f*5);
	for( int pp=0; pp<2; pp++)
	{
	glPushMatrix();
	//if (pp) glScalef(-1,1,1);
	//glBindTexture(GL_TEXTURE_2D, e_Text[ure[5]);
	e_Text[4].Use();
	glScalef(1,1,3);
	glTranslatef(-1.75f+pp*3.5f,-.65,0);

	glRotatef(90,0,1,0);

	glScalef(.75,.25,.05);
	glTranslatef(0,2,.4);
	for (int zx=0; zx<21; zx++)			// LIGHTS SIDE
	{

		glPushMatrix();
		glTranslatef(20.0f+zx*2.9f,0,0);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		//glBindTexture(GL_TEXTURE_2D, e_Text[ure[5]);
		e_Text[4].Use();


		glColor4f(1.0f,1.0f,1.0f,.25f+.75*var);
		glutSolidCube(1);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		if (e_timer>3) e_drawtrail(.75+1.5*var);
		else if (e_timer>2.625) e_drawtrail(2*(e_timer-2.625));
		glPopMatrix();
	}
	glPopMatrix();
	}

	glPushMatrix();
	//glBindTexture(GL_TEXTURE_2D, e_Text[ure[5]);
	e_Text[4].Use();
	glScalef(1.122,1,3);
	glTranslatef(0,2.591,0);
	glRotatef(90,1,0,0);
										// LIGHTS TOP
	glScalef(.25,.75,.05);
	glTranslatef(0,0,.4);
	for (int zx=0; zx<21; zx++)
	{

		glPushMatrix();
		glTranslatef(0,-20.0f-zx*3.0f,0);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		//glBindTexture(GL_TEXTURE_2D, e_Text[ure[5]);
		e_Text[4].Use();

		glColor4f(1.0f,1.0f,1.0f,.25f+.75*var);
		glutSolidCube(1);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		if (e_timer>3) e_drawtrailup(.75+1.5*var);
		else if (e_timer>2.625) e_drawtrailup(2*(e_timer-2.625));
		glPopMatrix();
	}
	glPopMatrix();
	}
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glTranslatef(0,0,1.9-e_zeta/1.75);

	//glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	if (e_timer<0.0f) e_radius=cos(e_timer*3.1415/2.0f-3.1415/2.0f)+1.0f;//+3.1415/2.0f)+1.0f;
	else
	if (e_timer<1.0f) e_radius=cos(e_timer*3.1415/2.0f+3.1415/2.0f)+1.0f;
	//else if (e_timer<1.5f) e_radius=.5f-.5f*sin((e_timer-1.0f)*3.1415f);
	//glClearColor(e_radius,e_radius,e_radius,1.0f);
	e_FogColor[0]=e_radius;
	e_FogColor[1]=e_radius;
	e_FogColor[2]=e_radius;
	glFogfv(GL_FOG_COLOR,e_FogColor);
	//glFogf(GL_FOG_DENSITY,e_radius*20);
	if (e_timer>16.5) glFogf(GL_FOG_DENSITY,.075+(e_timer-16.5)*(e_timer-16.5)*3.0);
	glFogf(GL_FOG_START,10-11*e_radius);

	if (e_timer<1.0f)
	{
	glLoadIdentity();		// LIGHT MASK
	glTranslatef(0,0,-.1);
	glRotatef(e_timer*200,0,0,1);
	glRotatef(180,1,0,0);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glEnable(GL_BLEND);
	glColor4f(1.0f,1.0f,1.0f,1.0f-e_timer/1.0f);
	//glBindTexture(GL_TEXTURE_2D, e_Text[ure[0]);
	e_Text[1].Use();
	glutSolidCone(.2,e_timer/3.5f,20,20);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	}

	if (e_lasers)				// e_lasers
	{
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		//glBindTexture(GL_TEXTURE_2D, e_Text[ure[13]);
		e_Text[4].Use();
		glDisable(GL_DEPTH_TEST);
		glLoadIdentity();
		glTranslatef(0,0,-.1);

		glRotatef(5*e_zrot,0,0,1);
		glRotatef(5*e_yrot,0,1,0);
		glRotatef(5*e_xrot,1,0,0);

		glPushMatrix();
		glRotatef(e_zeta*2.5f,0,0,1);
		glRotatef(179.5f,1,0,0);

		glColor4f(1.0f,1.0f,1.0f,.25f+(e_timer-16.0f)/8.0f);
		glutSolidCone(.25,100-(e_zeta/10.0f-100)/2.0f,10,5);
		glPopMatrix();

		//glBindTexture(GL_TEXTURE_2D, e_Text[ure[1]);

		e_Text[2].Use();

		glPushMatrix();
		glRotatef(-e_zeta*5.0f,0,0,1);
		glRotatef(179.25f,1,0,0);
		glColor4ub(128,255,128,64);
		glColor4f(0.25f,0.5f,1.0f,.5f);
	//	glColor4f(1.0f,1.0f,1.0f,.5f);
		glutSolidCone(.25,200-(e_zeta/1.5f-200),10,3);
		glPopMatrix();

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_2D);
	}
	}
	}
	else
	{
		if (e_timer<-1.25f)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			glDisable(GL_TEXTURE_2D);
			glLoadIdentity();
			glTranslatef(0,0,-1.0f);
			glColor4f(1,1,1,-2.0f*(e_timer+1.25f));
			e_drawquad(1.2);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		}
		else e_scene=true;
	}


	if ((e_timer>6.5)&&(e_timer<9.5))
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f+50*(.5f-.5f*cos((e_timer-6.5f)*3.1415f*2.0f/3.0f)),(GLfloat)width/(GLfloat)height,0.1f,90.0f);
		glMatrixMode(GL_MODELVIEW);
	}

	if ((e_timer>13)&&(e_timer<15))
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f+15*(.5f-.5f*cos((e_timer-13)*3.1415f/2.0f)),(GLfloat)width/(GLfloat)height,0.1f,90.0f);
		glMatrixMode(GL_MODELVIEW);
	}

	if (((e_timer>5)&&(e_timer<9.75))||((e_timer>12)&&(e_timer<14.1)))
	{
		if ((e_timer>6.5)&&(dum[1]))
		{
			dum[1]=false;//FSOUND_PlaySound(FSOUND_FREE, woosh);
		}
		if ((e_timer>12.4)&&(dum[3]))
		{
			dum[3]=false;//FSOUND_PlaySound(FSOUND_FREE, woosh1);
		}
	glLoadIdentity();
	//GLfloat acc=0;
	glTranslatef(.75f*sin(e_timer),.35f*cos(e_timer),0);
	glRotatef(5*e_zrot,0,0,1);
	glRotatef(5*e_yrot,0,1,0);
	glRotatef(5*e_xrot,1,0,0);
	//if (e_timer>7.7) acc=1.0f;
	//if (e_timer<7.7) glTranslatef(-.25-.5*sin(e_timer),.5-.5*cos(e_timer),46-6*e_timer-25*acc*(e_timer-7.7)*(e_timer-7.7));
	if (e_timer<7.7) glTranslatef(-.25-.5*sin(e_timer),.5-.5*cos(e_timer),46-6*e_timer);
	else if (e_timer<8.7) glTranslatef(-.25-.5*sin(e_timer),.5-.5*cos(e_timer),46-6*e_timer+4*(.5-.5*cos((e_timer-7.7)*3.1415)));
	else if (e_timer<11) glTranslatef(-.25-.5*sin(e_timer)+(e_timer-8.7)*(e_timer-8.7),.5-.5*cos(e_timer)-5*(e_timer-8.7)*(e_timer-8.7),50-6*e_timer-250*(e_timer-8.7)*(e_timer-8.7)*(e_timer-8.7));
	else glTranslatef(.5-.5*sin(e_timer),1-.25*(e_timer-12)*(e_timer-12),-4*(e_timer-12)*(e_timer-12)*(e_timer-12));
	glPushMatrix();
	if (e_timer<11) glRotatef(60*sin(e_timer*2.5)*sin(e_timer*1.5),0,0,1);
	else glRotatef(-70-120*sin((e_timer-11.7)*4.0),0,0,1);
	glDisable(GL_BLEND);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	e_Text[8].Use();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);
	//glPushMatrix();
	glScalef(1.5,1.5,1.5);
    glCallList(shiplist);
	glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	e_Text[1].Use();
	glTranslatef(0,0,.5);
	if (e_timer<7.7) glEnable(GL_DEPTH_TEST);
	glRotatef(-5*e_yrot,0,1,0);
	if (e_timer>8.6)
	{
		if (dum[2]) {dum[2]=false;//FSOUND_PlaySound(FSOUND_FREE, sweep);}
		}
		e_Text[9].Use();
		if (e_timer<13) glColor4f(1,1,1,.5);
		else glColor4f(1,1,1,.5-.5*(e_timer-13.0)*1.1);
		if (e_timer<8.7)
		{
			e_drawquad((e_timer-8.6)*20);
			glRotatef(e_timer*100,0,0,1);
			e_drawquad((e_timer-8.6)*50);
		}
		else
		{
			e_drawquad(2);
			glRotatef(e_timer*100,0,0,1);
			e_drawquad(5);
		}
	}
	glTranslatef(0,0,.05);
	if (e_timer<13) glColor4f(1,1,1,1);
	else glColor4f(1,1,1,1-1*(e_timer-13.0)*1.1);
	e_Text[1].Use();
	e_drawquad(1);
	}

	if ((e_timer>1.0f)&&(e_timer<2.0f)) e_speed=sin(3.1415f*.5f*(e_timer-1.0))*.125f;
	//if ((e_timer>2.0f)&&(e_timer<13.25f)) e_speed=.125f+.1f;

	e_zeta=e_speed*(e_timer-1.0f)*174.0f;

//	e_timer+=.005f;
//	e_timer=-1.75f+((GLfloat)(FSOUND_Stream_GetTime(stream)-limit))/3500.0f;

	if (e_timer>16.75f)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(0,0,-1.0f);
		glColor4f(1,1,1,4.0f*(e_timer-16.75f));
		e_drawquad(1.8);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		//glClearColor(-4.0f*(e_timer+1.0f),-4.0f*(e_timer+1.0f),-4.0f*(e_timer+1.0f),-4.0f*(e_timer+1.0f));
	}



	if (e_timer>17.0f)
	{
		//*********************************** FINISH
		//e_Clean();
		return false;
	}
	glutSwapBuffers();
	return true;
}
