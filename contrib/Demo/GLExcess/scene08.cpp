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
#include "scene08.h"

static Texture *g_Text;
static GLuint numtexs=18;
static GLuint width=800;
static GLuint height=600;
static bool init=true;
static GLfloat g_time=0;

static long limit=0;
static int g_check=2;

static float g_points[64][64];
static bool playjet=true;
static const int g_num=50;
static const int g_num1=250;
static long g_gettime=0;
static GLfloat g_ext=10.0f;

typedef struct
{
	GLfloat size,
			spd,
			z,
			fact,
			r,
			g_a;
}
g_part;

typedef struct
{
	GLfloat size,
			spd,
			h,
			r,
			g_a;
	long init;
}
g_part1;

static int g_scene=0;
static g_part parts[g_num];
static g_part1 parts1[g_num1];

static GLfloat g_litetop=1.0f;
static GLfloat g_liteleft=0.5f;
static GLfloat g_literite=0.25f;

static GLfloat g_rot=1.5f;
static GLfloat g_rota=0.0;
static GLfloat g_zeta=1;
static int g_phase[64][64];
static GLuint g_speed[64][64];
static GLuint g_gx,g_gy;
static GLuint g_a=0,g_b=0,g_c=1;
static GLfloat g_radius=-6.0f;

//static GLfloat g_FogColor[]={ 1.0f, 1.0f, 1.0f, 1.0f };

void g_Clean(void)
{
	g_Text[0].Kill();
	g_Text[1].Kill();
	g_Text[2].Kill();
	g_Text[3].Kill();
	g_Text[4].Kill();
	g_Text[5].Kill();
	g_Text[6].Kill();
	g_Text[7].Kill();
	g_Text[8].Kill();
	g_Text[9].Kill();
	g_Text[10].Kill();
	g_Text[11].Kill();
	g_Text[12].Kill();
	g_Text[13].Kill();
	g_Text[14].Kill();
	g_Text[15].Kill();
	g_Text[16].Kill();
	delete [] g_Text;
	init=true;
}

void g_drawquad(GLfloat size)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-.5*size,-.5*size,0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(.5*size,-.5*size,0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(.5*size,.5*size,0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-.5*size,.5*size,0);
	glEnd();
}

void g_drawquadr(GLfloat size)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-.5*size,-.5*size,0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(.5*size,-.5*size,0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(.5*size,.5*size,0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-.5*size,.5*size,0);
	glEnd();
}

void g_rst(int x)
{
		parts[x].size=.001*((GLfloat)(rand()%1000));
		parts[x].spd=.001*((GLfloat)(rand()%1000));
		parts[x].spd=.0005*((GLfloat)(rand()%1000));
		parts[x].fact=.00025*((GLfloat)(rand()%1000));
		parts[x].z=0.0f;
		parts[x].r=.5+.0005*((GLfloat)(rand()%1000));
		parts[x].g_a=.001*((GLfloat)(rand()%1000));
}

void g_rst1(int x)
{
		parts1[x].size=.001*((GLfloat)(rand()%1000));
		parts1[x].spd=.25+.00025*((GLfloat)(rand()%1000));
		parts1[x].init=g_gettime;//FSOUND_Stream_GetTime(stream);
		parts1[x].h=.001*((GLfloat)(rand()%1000));
		parts1[x].r=.5+.0005*((GLfloat)(rand()%1000));
		parts1[x].g_a=.001*((GLfloat)(rand()%1000));
}


void g_InitGL()
{
	g_Text=new Texture[numtexs];
	limit=0;
	g_check=2;

	g_scene=0;
	g_gettime=0;
	g_litetop=1.0f;
	g_liteleft=0.5f;
	g_literite=0.25f;

	g_rot=1.5f;
	g_rota=0.0;
	g_zeta=1;

	g_gx=0;g_gy=0;
	g_a=0,g_b=0,g_c=1;
	g_radius=-6.0f;

	g_litetop=1.0f;
	g_liteleft=0.5f;
	g_literite=0.25f;

	g_rot=1.5f;
	g_rota=0.0;
	g_zeta=1;
	g_gx=0;g_gy=0;
	g_a=0,g_b=0,g_c=1;
	g_radius=-6.0f;
	g_scene=0;
	g_ext=10.0f;
	playjet=true;
	//jet=FSOUND_Sample_LoadMpeg(FSOUND_FREE,"data/jet.mp3",FSOUND_NORMAL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,40.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_FOG);

	g_Text[0].Load("data/esaflr.raw");
	g_Text[1].Load("data/senv36.raw");  //MOON
	g_Text[2].Load("data/sky7.raw");	  //MOON
	g_Text[11].Load("data/env26.raw"); //DUSK
	g_Text[12].Load("data/sky27.raw"); //DUSK
	g_Text[13].Load("data/env17.raw"); //DAY
	g_Text[14].Load("data/sky17.raw"); //DAY
	g_Text[15].Load("data/basic2.raw");
	g_Text[16].Load("data/white.raw");
	g_Text[17].Load("data/moon1.raw");
	g_Text[3].Load("data/f16.raw");
	g_Text[4].Load("data/f16mask.raw");
	g_Text[5].Load("data/basic2.raw");
	g_Text[6].Load("data/sun2.raw");
	g_Text[7].Load("data/trail.raw");
	g_Text[8].Load("data/trailleft.raw");
	g_Text[9].Load("data/trailright.raw");
	g_Text[10].Load("data/floodmask.raw");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

	glShadeModel(GL_FLAT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
	glClearColor(.4862f, .4352f, .2627f, 0.5f);
	glClearColor(.0, .0, .0, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable (GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glFrontFace(GL_CCW);

	for(int x=0; x<64; x++)
	{
		for(int y=0; y<63; y++)
		{
			g_phase[x][y]=(int)(.001*(rand()%5000));
			g_speed[x][y]=(GLuint)(.1+.001*(rand()%10000));
		}
	}
	for (int x=0; x<64; x++)
	{
		g_phase[x][63]=g_phase[x][0];
		g_speed[x][63]=g_speed[x][0];
	}

	for (int x=0; x<g_num; x++)
	{
		g_rst(x);
	}

	for (int x=0; x<g_num1; x++)
	{
		g_rst1(x);
	}

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	g_zeta=66.0f+45.0f*((((GLfloat)g_gettime))/13000.0f)*((((GLfloat)g_gettime))/13000.0f);
	g_rota=-.3+((GLfloat)(g_gettime-limit))/2500.0f;
	g_rot=1.5f+(g_zeta-1.0f)/15.0f;
}

bool g_DrawGLScene(GLfloat globtime)
{
	if (init) {g_InitGL();init=false;}
	g_time=5*globtime;

	if (((g_scene==1)&&(g_check==2))||((g_scene==2)&&(g_check==1)))
	{
		g_check--;
		limit=g_gettime;
		g_a=0;
		g_b=0;
		g_c=1;
		//g_rota=-0.5f;
	}
//	if (g_rota<0.0f) glClearColor(-g_rota,-g_rota,-g_rota,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor4f(1,1,1,1);
	glLoadIdentity();
	//glRotatef(g_yrot,1.0f,0.0f,0.0f);
	//glRotatef(25,0.0f,0.0f,1.0f);
	glTranslatef(0,-3,-5+g_zeta);

	//gluLookAt(16,0,.05,16,16,0,0,1,0);
	  gluLookAt(16,0,.05,16,16,0,0,1,0);
	//glRotatef(45,1,0,0);
	glutSolidSphere(0,10,10);

	//glBindTexture(GL_TEXTURE_2D, g_Text[ure[0]);
	if (!g_scene) g_Text[1].Use();
	else if (g_scene==1) g_Text[11].Use();
	else g_Text[13].Use();
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	if (((unsigned int)g_zeta)/64==g_c)
	{
		if ((g_c%2)==0) g_b++; else g_a++;
		g_c++;
	}
	glPushMatrix();
	glTranslatef(-5,(126*g_a),0);
	glBegin(GL_QUADS);
	for( g_gx = 0; g_gx < 63; g_gx++ )
	{
		for( g_gy = 0; g_gy < 63; g_gy++ )
		{
			glVertex3f( float(g_gx)/1.5f, float(g_gy), g_points[g_gx][g_gy]);
			glVertex3f( float(g_gx+1)/1.5f, float(g_gy), g_points[g_gx+1][g_gy]);
			glVertex3f( float(g_gx+1)/1.5f, float(g_gy+1), g_points[g_gx+1][g_gy+1]);
			glVertex3f( float(g_gx)/1.5f, float(g_gy+1), g_points[g_gx][g_gy+1]);
		}
	}
	glEnd();
	glPopMatrix();
	glTranslatef(-5,63+(126*g_b),0);
	glBegin(GL_QUADS);
	for( g_gx = 0; g_gx < 63; g_gx++ )
	{
		for( g_gy = 0; g_gy < 63; g_gy++ )
		{
			glVertex3f( float(g_gx)/1.5f, float(g_gy), g_points[g_gx][g_gy]);
			glVertex3f( float(g_gx+1)/1.5f, float(g_gy), g_points[g_gx+1][g_gy]);
			glVertex3f( float(g_gx+1)/1.5f, float(g_gy+1), g_points[g_gx+1][g_gy+1]);
			glVertex3f( float(g_gx)/1.5f, float(g_gy+1), g_points[g_gx][g_gy+1]);
		}
	}
	glEnd();
	//glDisable(GL_FOG);
	glLoadIdentity();

	glTranslatef(0,-3,-40);
	glDisable(GL_CULL_FACE);

	//glBindTexture(GL_TEXTURE_2D, g_Text[ure[0]);
	if (!g_scene) g_Text[2].Use();
	else if (g_scene==1) g_Text[12].Use();
	else g_Text[14].Use();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glRotatef(g_rot,1,0,0);
	//glTranslatef(-8+g_rot/2,9+g_rot/2,0);
	glTranslatef(-0,8.8+g_rot/5,0);
	//glScalef(30,8+g_rot/2,1);
	glScalef(22.1,8.8+g_rot/5,1);
	glBegin(GL_QUADS);
		glTexCoord2d(0,1);
		glVertex3f(-1.0,-1.0,0.0);
		glTexCoord2d(1,1);
		glVertex3f(1.0,-1.0,0.0);
		glTexCoord2d(1,0);
		glVertex3f(1.0,1.0,0.0);
		glTexCoord2d(0,0);
		glVertex3f(-1.0,1.0,0.0);
	glEnd();
	for( g_gy = 0; g_gy < 64; g_gy++ ) for( g_gx = 0; g_gx < 64; g_gx++) g_points[g_gx][g_gy]=.025*sin(g_speed[g_gx][g_gy]*g_rota+g_phase[g_gx][g_gy]);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	g_Text[10].Use();
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	if (g_scene!=2) glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	else glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glLoadIdentity();
	if (!g_scene) glTranslatef(.15,-.11,-1.0f);//MOON
	else if (g_scene==1) glTranslatef(0,-.1075f,-1.0f);//DUSK
	else glTranslatef(0,-.095,-1.0f);//DAY
	if (!g_scene) glScalef(2.0,.07,1);
	else if (g_scene==1) glScalef(1.5,.07,1);
	else glScalef(2.5,.05,1);
	glColor4f(1,1,1,1);
	g_drawquadr(1);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);


	g_rota=-.3+((GLfloat)(g_gettime-limit))/2500.0f;
	g_rot=1.5f+(g_zeta-1.0f)/15.0f;
	//else if (!g_scene) g_rot=1.5f+(g_zeta-1.0f)/15.0f;

	glEnable(GL_CULL_FACE);
if ((g_radius>-6.0f)&&(g_radius<3.0f))
{
	glLoadIdentity();
	glTranslatef(-4,1,-5);

	glScalef(2,1.25,1);
	glRotatef(-45,0,0,1);

	glTranslatef(2*cos(g_radius),.5*sin(g_radius),-g_radius*1.5);

	glRotatef(45+g_radius*15,0,0,1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR,GL_ZERO);
	//glBindTexture(GL_TEXTURE_2D, g_Text[ure[3]);
	g_Text[4].Use();
	g_drawquad(1);

	glBlendFunc(GL_ONE,GL_ONE);
	//glBindTexture(GL_TEXTURE_2D, g_Text[ure[2]);
	g_Text[3].Use();
	g_drawquad(1);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);/////////////////////////////////////////
	glColor4f(1.0f,1.0f,1.0f,g_litetop);
	//glBindTexture(GL_TEXTURE_2D, g_Text[ure[6]);
	g_Text[7].Use();
	g_drawquad(1);////////////////////////////////////////////
	g_litetop-=.05f;
	if (g_litetop<0) g_litetop=1.0f;

	glColor4f(.10f,.75f,.25f,g_liteleft);
	//glBindTexture(GL_TEXTURE_2D, g_Text[ure[7]);
	g_Text[8].Use();
	g_drawquad(1);////////////////////////////////////////////
	g_liteleft-=.25f;
	if (g_liteleft<-2) g_liteleft=1.0f;

	glColor4f(1.0f,.25f,.25f,g_liteleft);
	//glBindTexture(GL_TEXTURE_2D, g_Text[ure[8]);
	g_Text[9].Use();
	g_drawquad(1);////////////////////////////////////////////
	g_literite-=.1f;
	if (g_literite<-1.5) g_literite=1.0f;

	glLoadIdentity();
	glTranslatef(-4,1,-5);
	glScalef(2,1.25,1);
	glRotatef(-45,0,0,1);
	glTranslatef(2*cos(g_radius),.5*sin(g_radius),-g_radius*1.5);

	glRotatef(-45,0,0,1);

	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, g_Text[ure[4]);
	g_Text[5].Use();
	//glDisable(GL_FOG);

	glDisable(GL_DEPTH_TEST);

	glLoadIdentity();
	if (g_radius<0.0f) glTranslatef(-4.075,.8,-5);
	else glTranslatef(-4.075+g_radius/11,.8,-5);
	glScalef(2,1.25,1);
	glRotatef(-45,0,0,1);
	glTranslatef(2*cos(g_radius),.5*sin(g_radius),-g_radius*1.5);

	glRotatef(45,0,0,1);
	glRotatef(20+g_radius*20,1,0,0);

	if (g_radius<0.0f) glRotatef(-5,0,1,0);
	else glRotatef(-5+12.5*g_radius,0,1,0);

	for (int i=0; i<g_num+1; i++)
	{
		glPushMatrix();
		if (i==g_num)
		{
			glColor4f(1,1,1,.25+.0005*((GLfloat)(rand()%1000)));
			glRotatef(20+g_radius*20,1,0,0);
			if (g_radius<0.0f) glRotatef(5,0,1,0);
			else glRotatef(5-12.5*g_radius,0,1,0);
			glRotatef(-20-g_radius*20,1,0,0);
			//glBindTexture(GL_TEXTURE_2D, g_Text[ure[5]);
			g_Text[6].Use();
			glScalef(.35,.7,1);
			glRotatef(-g_radius*50,0,0,1);
			g_drawquad(1+cos(g_radius)*cos(g_radius));
		}
		else
		{
		glTranslatef(0,0,parts[i].z/25);
		glColor4f(parts[i].r,parts[i].r/2,parts[i].r/4,parts[i].g_a);
		glRotatef(-5-g_radius*8,0,1,0);
		glRotatef(-20-g_radius*20,1,0,0);
		glScalef(.35,.5,1);
		g_drawquad(parts[i].size/2);
		parts[i].g_a-=.15;
		parts[i].z+=parts[i].spd*5;
		if ((parts[i].g_a<0.0f)||(parts[i].size<0.0f)) g_rst(i);
		}
		glPopMatrix();
	}

	glLoadIdentity();
	glTranslatef(-3+2*cos(g_radius*1.25),-1,-2-2*g_radius);
	glColor4f(.2,.2,.2,.5);
	glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_COLOR);
	glRotatef(-90,1,0,0);
	//glBindTexture(GL_TEXTURE_2D, g_Text[ure[4]);
	g_Text[5].Use();
	glRotatef(25*g_radius,0,0,1);
	glScalef(1,3,1);
	g_drawquad(1);
}
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	if (!g_scene) g_zeta=66.0f+45.0f*((((GLfloat)g_gettime-limit))/13000.0f)*((((GLfloat)g_gettime-limit))/13000.0f);
	else if (g_scene==1) g_zeta=15.0f+25.0f*(((GLfloat)g_gettime-limit)/1500.0f);
	else if (g_rota<3.0f) g_zeta=15.0f+25.0f*(((GLfloat)g_gettime-limit)/1500.0f);
	else g_zeta=15.0f+25.0f*(((GLfloat)g_gettime-limit)/1500.0f)-30.0f*(1.0f-cos((g_rota-3.0f)/5.0f*3.1415f));

	if ((g_scene==2)&&(g_radius<3)) g_radius=-6.0f+(g_zeta-20.0f)*(g_zeta-20.0f)/2000.0f;

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	g_Text[0].Use();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glLoadIdentity();

	if (!g_scene) glTranslatef(.23,-.06f,-1.0f); // MOON
	else if(g_scene==1) glTranslatef(.07,-.07f,-1.0f); // DUSK
	else glTranslatef(.325,-.08f,-1.0f); // SKY

	if (g_scene==1) g_ext=10.0f; else g_ext=10.0f;

	for (int l=0; l<g_num1; l++)
	{
		GLfloat time;
		time=((GLfloat)(g_gettime-parts1[l].init))/1000.0f;
		if (50.0f*parts1[l].spd*time>2.0f*3.1415f) g_rst1(l);
		else
		{
			glPushMatrix();
			if (!g_scene) glColor4f(parts1[l].r/4,parts1[l].r/2,parts1[l].r,.5f*parts1[l].h*parts1[l].g_a*(1.0f-cos(50.0f*parts1[l].spd*time))); // MOON
			else
			if (g_scene==1) glColor4f(parts1[l].r,parts1[l].r,parts1[l].r/2,.5f*parts1[l].h*parts1[l].g_a*(1.0f-cos(50.0f*parts1[l].spd*time)));   // DUSK
			else
							glColor4f(parts1[l].r/4,parts1[l].r/2,parts1[l].r,1.0f*parts1[l].h*parts1[l].g_a*(1.0f-cos(50.0f*parts1[l].spd*time)));   // DUSK

			if ((l%2)==0) glTranslatef(parts1[l+1].h*parts1[l].h/g_ext,0,0);
			else glTranslatef(-parts1[l-1].h*parts1[l].h/g_ext,0,0);

			if (l>(int)g_num1*(3.0f/4.0f)) glTranslatef(0,-parts1[l].h/3.0f,0);
			else
			if (l>(int)g_num1*(2.0f/4.0f)) glTranslatef(0,-parts1[l].h/4.0f,0);
			else
			if (l>(int)g_num1*(1.0f/4.0f)) glTranslatef(0,-parts1[l].h/5.0f,0);
			else glTranslatef(0,-parts1[l].h/6.0f,0);
			glRotatef(500.0f*time*parts1[l].spd,0,0,1);
			g_drawquad(.015f+((1.0f-cos(50.0f*parts1[l].spd*time)))*parts1[l].size*parts1[l].h/30.0f);
			if (l<g_num1/10)
			{
				glLoadIdentity();
				if (!g_scene) glTranslatef(.42,-.25f,-1.0f); // MOON
				else if(g_scene==1) glTranslatef(.12,-.22f,-1.0f); // DUSK
				else glTranslatef(.6,-.2f,-1.0f); // DAY
				glTranslatef(0,-((GLfloat)l/(g_num1/10))/1.5f,-1.0f);
				if (!g_scene) glColor4f(.25f,.75f,1.0f,.25f);// MOON
				else if(g_scene==1) glColor4f(1.0f,1.0f,.5f,.5f);// DUSK
				else glColor4f(.25f,.75f,1.0f,.25f);// DAY
				if ((l%2)==0) glTranslatef(((GLfloat)l/(g_num1/10))/10.0f+parts1[l].h/30.0f,0,0);
				else glTranslatef(((GLfloat)l/(g_num1/10))/10.0f-parts1[l].h/30.0f,0,0);
				g_drawquad(.015f+((1.0f-cos(50.0f*parts1[l].spd*time)))*parts1[l].size*parts1[l].h/1.5f);
			}
			glPopMatrix();
		}
	}

	GLfloat cips=(((GLfloat)g_gettime-limit))/1500.0f;
	glLoadIdentity();
	if (!g_scene)
	{
		g_Text[15].Use();
		glTranslatef(.2+g_rota/1000.0f,.185+g_rota/1100.0f,-1.0f);
		glColor4f(1,1,1,.5f);
		g_drawquad(.2f+.05*sin(cips));
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
		glColor4f(1,1,1,1);
		g_Text[17].Use();
		g_drawquad(.08f);
	}

	else if (g_scene==1)
	{
		g_Text[16].Use();
		if (g_scene==1) glTranslatef(.068,.04+g_rota/800.0f,-1.0f);
		//else glTranslatef(.3,.5,-1.0f);
		glColor4f(1,1,1,.35f);
		//glColor4f(0,0,0,0);
		glRotatef(sin(cips/2.0f)*50,0,0,1);
		g_drawquad(.4f+.75*sin(cips/1.5f)*sin(cips/1.5f));
		glRotatef(sin(cips/4.0f)*100,0,0,1);
		g_Text[0].Use();
		g_drawquad(.2f+.3*cos(cips)*cos(cips));
	}
	else
	{
		g_Text[16].Use();
		//if (g_scene==1) glTranslatef(.068,.04,-1.0f);
		//else
		glTranslatef(.3,.3,-1.0f);
		glColor4f(1,1,1,.3f);
		glRotatef(sin(cips/2.0f)*50,0,0,1);
		g_drawquad(.3f+.5*sin(cips/1.5f)*sin(cips/1.5f));
		glRotatef(sin(cips/4.0f)*100,0,0,1);
		g_Text[0].Use();
		g_drawquad(.2f+.3*cos(cips)*cos(cips));
		g_Text[15].Use();
		g_drawquad(.3f);//+.1*cos(cips)*cos(cips));
	}

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	if ((g_rota<1.0f)||
		((g_rota>9.2f)&&(!g_scene))||
		((g_rota>5.7f)&&(g_scene==1))||
		((g_rota>4.0f)&&(g_scene==2))
		)
	{
		if ((!g_scene)&&(g_rota<1.0f)) glBlendFunc(GL_ZERO,GL_ONE_MINUS_SRC_ALPHA);
		else glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glLoadIdentity();
		glTranslatef(0,0,-.9f);
		if (g_rota<0) glColor4f(1,1,1,1); else
		if (g_rota<1.0f) glColor4f(1,1,1,.5f*(1.0+cos(g_rota*3.1415f)));
		if (g_rota>9.2) glColor4f(1,1,1,.5f*(1.0+cos(-3.1415f+(g_rota-9.2f)*4.0f*3.1415f)));
		if ((g_scene==1)&&(g_rota>5.7f)) glColor4f(1,1,1,.5f*(1.0+cos(-3.1415f+(g_rota-5.7f)*4.0f*3.1415f)));
		if ((g_scene==2)&&(g_rota>4.0f)) glColor4f(1,1,1,.5f*(1.0+cos(-3.1415f+(g_rota-4.0f)*3.1415f)));
		glDisable(GL_TEXTURE_2D);
		glScalef(1.1,.8,1);
		g_drawquad(1);
		glEnable(GL_TEXTURE_2D);
	}
	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	if ((g_scene==2)&&(g_rota>1.6)&&(playjet))
	{
		playjet=false;
//		FSOUND_PlaySound(FSOUND_FREE, jet);
	}
	//if (((!g_scene)&&(g_rota>11.75))||
	if (((!g_scene)&&(g_rota>9.45))||
		((g_scene==1)&&(g_rota>5.95))
		)
		g_scene++;


	if ((g_scene==2)&&(!g_check)&&(g_rota>5.0f))
	{
		//******************* FINISH
		//g_Clean();
		return false;
	}
	//MessageBox(NULL,"","",0);
	g_gettime=(long int)g_time;
	glutSwapBuffers();
	return true;
}
