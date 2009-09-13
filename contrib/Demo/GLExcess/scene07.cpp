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
#include "scene07.h"
#include "scene07-data.h"

static Texture* f_Text;
static GLuint numtexs=7;
static GLuint width=800;
static GLuint height=600;
static bool init=true;
static GLfloat f_time=0;

static GLuint f_cycle;

static int f_a,f_b,f_c,f_d;

static bool f_play=true;bool f_play1=true;bool f_play2=true;

static GLfloat f_rot=0.0;
static GLfloat f_timer=0.0f;
static GLfloat f_factor=1.0f;
static long f_frames=0;
static GLfloat f_zeta=-15.0f;//.001;
static GLfloat f_end=1.0;
static int f_phase[64][64];
static GLuint f_speed[64][64];

static int f_shade;
static int f_shadetop;
static int f_flare;

static GLuint f_side[10][10][10];
static const int f_num=500;
static const int f_acn=250;

typedef struct
{
	GLuint alfa,f_shade;
	GLfloat mod,f_speed;
}
f_particle;

typedef struct
{
	GLuint arot;
	long ainit;
	GLfloat amod,aspeed,aalfa,arad;
}
f_acc;

static f_particle particles[f_num];
static f_acc accs[f_acn];

static GLfloat f_angle[10][10][10];

static GLfloat f_FogColor[]={ 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat f_density=0.025;

void f_initacc(int naccs)
{
	accs[naccs].amod=.001*(float(rand()%1000));
	accs[naccs].arot=rand()%360;
	accs[naccs].aalfa=.001*(float(rand()%1000));
	accs[naccs].aspeed=.00075*(float(rand()%1000));
	accs[naccs].ainit=(long int)f_time;
	accs[naccs].arad=.05+.00035*(float(rand()%1000));
}

void f_InitGL()
{
	f_Text=new Texture[numtexs];
	f_density=0.025;
	f_cycle=0;
	f_a=0;f_b=0;f_c=0;f_d=0;
	f_play=true;
	f_play1=true;
	f_play2=true;

	f_zeta=-15.0f;
	f_shade=0;
	f_shadetop=0;
	f_flare=0;


	f_rot=0.0;
	f_timer=0.0f;
	f_factor=1.0f;
	f_frames=0;

	f_end=1.0;

	f_frames=0;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//	FILE* f_in=NULL;
//	f_in=fopen("Data\\data","r");

	f_Text[1].Load("data/text.raw");
	f_Text[2].Load("data/white.raw");
	f_Text[3].Load("data/circle.raw");
	f_Text[4].Load("data/circleempty.raw");
	f_Text[5].Load("data/circlefill.raw");
	f_Text[6].Load("data/sground.raw");
	glShadeModel(GL_FLAT);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable (GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for(int f_x=0; f_x<64; f_x++)
	{
		for(int f_y=0; f_y<63; f_y++)
		{
			f_phase[f_x][f_y]=(int)(.001*(rand()%5000));
			f_speed[f_x][f_y]=(GLuint)(.001*(rand()%10000));
		}
	}
	for (int f_x=0; f_x<64; f_x++)
	{
		f_phase[f_x][63]=f_phase[f_x][0];
		f_speed[f_x][63]=f_speed[f_x][0];
	}

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glFogf(GL_FOG_MODE,GL_EXP2);
	glFogf(GL_FOG_DENSITY,.025);
	f_FogColor[0]=0.0;
	f_FogColor[1]=0.0;
	f_FogColor[2]=0.0;
	glFogfv(GL_FOG_COLOR,f_FogColor);
	glEnable(GL_FOG);
	glEnable(GL_TEXTURE_2D);

	for (int i=0; i<10; i++)
		for (int j=0; j<10; j++)
			for (int k=0; k<10; k++)
			{
				f_a=datas[(i*100+j*10+k)*4];
				f_b=datas[(i*100+j*10+k)*4+1];
				f_c=datas[(i*100+j*10+k)*4+2];
				f_d=datas[(i*100+j*10+k)*4+3];

				//f_a-=48;				f_b-=48;				f_c-=48;				f_d-=48;

				f_angle[i][j][k]=100*f_a+10*f_b+f_c;
				f_side[i][j][k]=f_d;
			}

	for (int f_cycle=0;f_cycle<f_num;f_cycle++)
	{
		particles[f_cycle].mod=0.0;
		particles[f_cycle].alfa=rand()%360;
		particles[f_cycle].f_shade=rand()%128;
		particles[f_cycle].f_speed=.00000075*(float(rand()%10000));
	}

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	for (int f_cycle=0;f_cycle<f_acn;f_cycle++) f_initacc(f_cycle);
}

void f_Clean(void)
{
	f_Text[1].Kill();
	f_Text[2].Kill();
	f_Text[3].Kill();
	f_Text[4].Kill();
	f_Text[5].Kill();
	f_Text[6].Kill();
	delete [] f_Text;
	init=true;
}

void f_drawquad(GLfloat size)
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
bool f_DrawGLScene(GLfloat globtime)
{
	if (init) {f_InitGL();init=false;}
	f_time=10*globtime;
	if (f_zeta<32.5f)
	{
	if (f_timer<1.0f) f_frames++;
	else f_factor=20.0f/(GLfloat)f_frames;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glLoadIdentity();
	gluLookAt(5,5,-3+f_zeta,10,10,10,0,1,0);

			glPushMatrix();
			glTranslatef(10,10,10);
			f_Text[1].Use();
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			glScalef(30,30,30);
			glFrontFace(GL_CW);
			glColor3ub(128,160,192);
			glPushMatrix();
			glRotatef(f_rot,1,0,0);
			glutSolidSphere(1,50,50);
			glPopMatrix();
			glPopMatrix();

	glFrontFace(GL_CCW);
	f_Text[1].Use();
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glColor4ub(255,255,255,128);
	for (int f_x=9; f_x>=0; f_x--)
		for (int f_y=9; f_y>=0; f_y--)
			for (int f_z=9; f_z>=0; f_z--)
			{
				glPushMatrix();
				glTranslatef(2*f_x/f_end,2*f_y/f_end,2*f_z/f_end);
				if (f_side[f_x][f_y][f_z]==0) glRotatef(f_rot,1,0,0);
				if (f_side[f_x][f_y][f_z]==1) glRotatef(f_rot,0,1,0);
				if (f_side[f_x][f_y][f_z]==2) glRotatef(f_rot,0,0,1);
				if (f_side[f_x][f_y][f_z]==3) glRotatef(f_rot,0,1,1);
				if (f_side[f_x][f_y][f_z]==4) glRotatef(f_rot,1,0,1);
				if (f_side[f_x][f_y][f_z]==5) glRotatef(f_rot,1,1,0);
				if (f_side[f_x][f_y][f_z]==6) glRotatef(f_rot,1,1,1);

				if ((f_timer>12.75)&&(f_x==3)&&(f_y==3)&&(f_z==4)&&(f_timer<17))
				{
					glColor4f(1,.65,.35,.75);
					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);
					glEnable(GL_BLEND);
					GLfloat value=(f_angle[f_x][f_y][f_z]/(360*2));
					int tot=5;
					for (int times=0; times<tot; times++)
					{
						if (times==0) glColor4f(1,.65,.35,1.0);
						glColor4f(1,.65,.35,.5-.5*((GLfloat)times)/tot);
						glutSolidCube(value+.035*((GLfloat)times)/tot);
					}
					glDisable(GL_TEXTURE_GEN_S);
					glDisable(GL_TEXTURE_GEN_T);
					if(1)
					{
						f_Text[5].Use();
						glColor4f(1,1,1,1);
						glRotatef(-70,0,1,0);
						glRotatef(45,1,0,0);
						long tm;
						tm=(long int)f_time;
						for (int nac=0; nac<f_acn; nac++)
						{
							GLfloat asker=1.5*accs[nac].aspeed*((GLfloat)(tm-accs[nac].ainit)*(tm-accs[nac].ainit))/1000000.0;
							glPushMatrix();
							glRotatef(accs[nac].arot,0,0,1);
							glTranslatef(.35*(accs[nac].amod-asker),0,0);
							glColor4f(.5,.5,.5,accs[nac].aalfa*asker*3.5);
							f_drawquad(accs[nac].arad/8.0+asker/10.0);
							glPopMatrix();
							if (.35*(accs[nac].amod-asker)<0) f_initacc(nac);
						}
						glColor4f(1,1,1,1);
						f_drawquad(.2+.1*sin(f_timer));
					}
					glEnable(GL_DEPTH_TEST);
					glEnable(GL_CULL_FACE);
					glDisable(GL_BLEND);
					glColor4ub(255,255,255,128);
				}
				else
				glutSolidCube((f_angle[f_x][f_y][f_z]/(360*2)));
				glPopMatrix();
			}

			glFrontFace(GL_CW);

			glLoadIdentity();
			glTranslatef(0,0,-15);
			glEnable(GL_BLEND);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_DEPTH_TEST);
			if (f_zeta>0.0f) f_shadetop=(int)(255*(sin((f_zeta/40)*1.5*3.1415)));
			glRotatef(-100*sqrt(sqrt(f_zeta*f_zeta)),0,0,1);
			if (((f_zeta>0)&&(f_zeta<.9))||
				((f_zeta>2.2)&&(f_zeta<3.5))||
				((f_zeta>4.75)&&(f_zeta<6.05))||
				((f_zeta>7.6)&&(f_zeta<8.3))||
				((f_zeta>10)&&(f_zeta<10.8))||
				((f_zeta>12.7)&&(f_zeta<13.0))||
				((f_zeta>15.3)&&(f_zeta<15.6))||
				((f_zeta>20.3)&&(f_zeta<20.5))||
				((f_zeta>22.2)&&(f_zeta<23.5)))
			{
				if ((f_zeta>10)&&(f_zeta<10.8)) f_factor=f_factor/4.5;
				f_shade-=(int)(20.0f*f_factor);
				if (f_shade<0) f_shade=0;
				f_flare-=(int)(30.0f*f_factor);
				if (f_flare<0) f_flare=0;
			}
			else
			{
				if ((f_zeta>10.8)&&(f_zeta<12.7)) f_factor=f_factor/2.0;
				f_shade+=(int)(20.0f*f_factor);
				if (f_shade>f_shadetop) f_shade=f_shadetop;
				if (f_zeta>0.0f) f_flare+=(int)(40.0f*f_factor);
				if (f_flare>255) f_flare=255;
			}
			glPushMatrix();
			glRotatef(30,0,0,1);
			glTranslatef(-.2*f_zeta,0,0);
			glColor4ub(128,96,64,f_shade);

			f_Text[5].Use();
			f_drawquad(1.75);
			glColor4ub(255,255,255,f_shade);
			f_drawquad(1);

			f_Text[4].Use();
			glTranslatef(-.1*f_zeta,0,0);
			glColor4ub(192,64,64,f_shade);
			f_drawquad(2.75);

			f_Text[5].Use();
			glTranslatef(-.15*f_zeta,0,0);
			glColor4ub(128,212,64,f_shade);
			f_drawquad(.5);

			f_Text[3].Use();
			glTranslatef(.35*f_zeta,0,0);
			glColor4ub(128,128,128,f_shade/2);
			f_drawquad(.5);
			glTranslatef(-.025*f_zeta,0,0);
			glColor4ub(96,128,192,f_shade/2);
			f_drawquad(1);
			glPopMatrix();

			glPushMatrix();
			glRotatef(30,0,0,1);
			glTranslatef(.15*f_zeta,0,0);
			glColor4ub(128,128,128,f_shade/2);
			f_drawquad(1.75);
			glTranslatef(.05*f_zeta,0,0);
			f_drawquad(1.25);

			f_Text[5].Use();
			glTranslatef(.1*f_zeta,0,0);
			glColor4ub(64,64,192,f_shade);
			f_drawquad(1);
			glColor4ub(64,128,64,f_shade);
			f_drawquad(.8);
			glColor4ub(64,64,128,f_shade);
			f_drawquad(.6);
			glPopMatrix();

			f_Text[6].Use();
			glColor4ub(255,255,255,(unsigned char)(((GLfloat)f_flare)/1.5));
			glPushMatrix();
			glRotatef(3*f_rot,0,0,1);
			f_drawquad(5+10*cos(f_rot/10)*cos(f_rot/10));
			f_Text[2].Use();
			glColor4ub(255,255,255,f_flare);
			f_drawquad(5+5*cos(f_rot/10)*cos(f_rot/10));
			glRotatef(f_rot,0,0,1);
			f_Text[2].Use();
			f_drawquad(2+5*sin(f_rot/10)*sin(f_rot/10));
			f_Text[5].Use();
			glColor4ub(255,255,255,f_shade/2);
			glPopMatrix();
			f_drawquad(30);

	if ((f_shadetop<0)&&(f_zeta>0.0f))
	{
/*		if (f_play)
		{
			FSOUND_PlaySound(FSOUND_FREE, exps);
			f_play=false;
		}*/

		for (int f_cycle=0;f_cycle<f_num;f_cycle++)
		{
			glPushMatrix();
			glRotatef(particles[f_cycle].mod*10.0f*(1.0f+100.0*particles[f_cycle].f_speed),0,0,1);//(GLfloat)f_cycle/f_num),0,0,1);
			glRotatef(particles[f_cycle].alfa,0,0,1);
			glTranslatef((GLfloat)f_cycle*(1.0f+2.0f*(f_zeta-28.5f))/(GLfloat)f_num+particles[f_cycle].mod,0,0);
			glColor4ub(f_cycle/2,f_cycle/2,f_cycle/2,particles[f_cycle].f_shade);
			if (f_zeta>28.5f) f_drawquad(.1+.5*(float (particles[f_cycle].f_shade)/128));
			glPopMatrix();
			particles[f_cycle].mod=(f_zeta-28.5f)*(f_zeta-28.5f)/1.2f;
			f_end+=.000005*f_factor;
		}


	}
	f_rot=15.0f*f_timer;

	if (f_zeta>29.5)
	{
/*		if (f_play1)
		{
			FSOUND_PlaySound(FSOUND_FREE, revs);
			f_play1=false;
		}
*/
		f_density=.025f*(1.0f+(f_zeta-29.5f)/10.0f);
		glFogf(GL_FOG_DENSITY,f_density);
		glLoadIdentity();
		glTranslatef(0,0,-1);
		glColor4f(1.0f,1.0f,1.0f,(f_zeta-29.5f)/3.0f);

		glDisable(GL_TEXTURE_2D);
		f_drawquad(1.5);
		glEnable(GL_TEXTURE_2D);

	}
	}
	else
	{
		glClearColor(1.0f-2.0f*(f_zeta-32.5f),1.0f-2.0f*(f_zeta-32.5f),1.0f-2.0f*(f_zeta-32.5f),1);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	if (f_timer<1.0f)
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(0,0,-.35f);
		glColor4f(1.0f,1.0f,1.0f,1.0f-f_timer*2.0f);
		f_drawquad(.5f);
		glEnable(GL_TEXTURE_2D);
	}

	if ((f_timer>2.1f)&&(f_timer<3.1f))
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(0,0,-.35f);
		glColor4f(1.0f,1.0f,1.0f,.45*(1.0-cos((f_timer-2.1)*3.1415*2.0)));
		f_drawquad(.5f);
		glEnable(GL_TEXTURE_2D);
	}

	if ((f_timer>12.25f)&&(f_timer<13.25f))
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glLoadIdentity();
		glTranslatef(0,0,-.35f);
		glColor4f(1.0f,1.0f,1.0f,.45*(1.0-cos((f_timer-12.25)*3.1415*2.0)));
		f_drawquad(.5f);
		glEnable(GL_TEXTURE_2D);
	}

	if ((f_timer>22.3f)&&(f_timer<28.3))
	{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (f_timer<23.3f) gluPerspective(45.0f+25*(1.0-cos((f_timer-22.3)*3.1415)),(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	else gluPerspective(45.0f+25*(1.0+cos((f_timer-23.3)*3.1415/5.0)),(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);

/*	if ((f_timer>12.2)&&(f_play2))
	{
		FSOUND_PlaySound(FSOUND_FREE, pby);
		f_play2=false;
	}
*/	if (f_timer<1.4f) f_zeta=-15.0f+15.0f*(sin(f_timer*3.1415f/2.8f));//cos(3.1415f+f_timer*3.1415f/2.0f));
	else f_zeta=0.0f+(f_timer-1.4f)/1.24;
	f_timer=(f_time)/1500.0f;
	if (f_zeta>33.0f)
	{
		//************************** FINISH
		//f_Clean();
		return false;
	}
	glutSwapBuffers();
	return true;
}
