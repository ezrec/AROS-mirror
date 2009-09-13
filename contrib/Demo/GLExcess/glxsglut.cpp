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

#include <stdio.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <math.h>
#include "Texture.h"
#include "utils.h"
#include "scene01.h"
#include "scene02.h"
#include "scene03.h"
#include "scene04.h"
#include "scene05.h"
#include "scene06.h"
#include "scene07.h"
#include "scene08.h"
#include "scene09.h"
#include "scene10.h"
#include "scene11.h"
#include "scene12.h"

static bool loop=true;
static bool timerset=false;
static GLfloat timing=0;
static GLfloat step=1;
static GLuint scene=1;

void drawscene()
{
	switch (scene)
	{
		case 1:
			if (!z_DrawGLScene(timing)) {scene++; z_Clean(); timing=0;}
			break;
		case 2:
			if (!a_DrawGLScene(timing)) {scene++; a_Clean(); timing=0;}
			break;
		case 3:
			if (!b_DrawGLScene(timing)) {scene++; b_Clean(); timing=0;}
			break;
		case 4:
			if (!d_DrawGLScene()) {scene++; d_Clean(); timing=0;}
			break;
		case 5:
			if (!c_DrawGLScene(timing)) {scene++; c_Clean(); timing=0;}
			break;
		case 6:
			if (!e_DrawGLScene(timing)) {scene++; e_Clean(); timing=0;}
			break;
		case 7:
			if (!f_DrawGLScene(timing)) {scene++; f_Clean(); timing=0;}
			break;
		case 8:
			if (!g_DrawGLScene(timing)) {scene++; g_Clean(); timing=0;}
			break;
		case 9:
			if (!h_DrawGLScene(timing)) {scene++; h_Clean(); timing=0;}
			break;
		case 10:
			if (!i_DrawGLScene(timing)) {scene++; i_Clean(); timing=0;}
			break;
		case 11:
			if (!j_DrawGLScene(timing)) {scene++; j_Clean(); timing=0;}
			break;
		case 12:
			if (!k_DrawGLScene(timing)) {scene++; k_Clean(); timing=0;}
			break;
		case 13:
			if (loop) {scene=1; timing=0;}
			else exit(0);
			break;
	}
}
void update(int dummy)
{
	timing+=3;
	timerset=false;
}
void scenemanagement()
{
	timing+=step;
	drawscene();
	//if (!timerset) {glutTimerFunc(1,update,0);timerset=true;}
}
void display(void)
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glPointSize(5);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex3f(-.5,-.5,0);
	glTexCoord2f(1,0);
	glVertex3f(.5,-.5,0);
	glTexCoord2f(1,1);
	glVertex3f(.5,.5,0);
	glTexCoord2f(0,1);
	glVertex3f(-.5,.5,0);
	glEnd();
	glColor4f(1,1,1,1);
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x1, int y)
{
	switch (key)
	{
		case ' ':
			switch (scene)
			{
				case 1:	z_Clean(); break;
				case 2: a_Clean(); break;
				case 3: b_Clean(); break;
				case 4: d_Clean(); break;
				case 5: c_Clean(); break;
				case 6: e_Clean(); break;
				case 7: f_Clean(); break;
				case 8: g_Clean(); break;
				case 9: h_Clean(); break;
				case 10: i_Clean(); break;
				case 11: j_Clean(); break;
				case 12: k_Clean(); break;
			}
			scene++;timing=0;
			break;

		case 'a':
			step+=1;
			break;

		case 'z':
			step-=1;
			if (step<-1) step=-1;
			break;

		case 27:
			exit(0);
			break;

		default:
			glutPostRedisplay();
			break;
	}
}

int main(int argc, char** argv)
{
	int i;
	int use_glutfullscreen = 0;
	printf("argc=%d\n",argc);
	for (i=0; i<argc; i++) printf("argv[%d]:%s\n",i,argv[i]);
	int wparams[5]={0,640,480,24,60};
	for (i=0; ((i<argc-1)&&(i<5)); i++) wparams[i]=toint(argv[i+1]);
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	if (wparams[0])
	{
		printf ("Trying to use the glutEnterGameMode to obtain fullscreen (%s)\n", buildgamestring(wparams));

		glutGameModeString(buildgamestring(wparams));
		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
		{
			glutEnterGameMode();
		}
		else
		{
			printf("Game mode not available\n");
			printf("Using glutFullScreen instead, width and height might be different than requested\n");
			use_glutfullscreen = 1;
		}
	}

	glutInitWindowPosition (0, 0);
	glutInitWindowSize (wparams[1], wparams[2]);
	glutCreateWindow("GLExcess");
	if (use_glutfullscreen) glutFullScreen();

	glutPostRedisplay();

	glutKeyboardFunc(keyboard);
	glutDisplayFunc(scenemanagement);
	glutIdleFunc(scenemanagement);

	glutMainLoop();
	return 0;
}
