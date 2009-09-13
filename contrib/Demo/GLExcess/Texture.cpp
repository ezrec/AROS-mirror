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

#include "Texture.h"

Texture::Texture() {tID=0;}
Texture::~Texture(){Kill();}
bool Texture::Create(char* FileName)
{
	Kill();
	glGenTextures(1, &tID);
	tID++; // Use an offset of +1 to differentiate from non-initialized state.
	glBindTexture(GL_TEXTURE_2D, tID-1);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	return true;
}

void Texture::Kill()
{
	if(tID)
	{
		tID--;
		glDeleteTextures(1, &tID);
		tID=0;
	}
}
GLuint pow2(GLuint exp)
{
	GLuint result=1;
	if (!exp) return result;
	for (GLuint a=0; a<exp; a++)
	{
		result*=2;
	}
	return result;
}
void Texture::Use()
{
	glBindTexture(GL_TEXTURE_2D,tID-1);
}
bool Texture::Load(char* Opaque)
{
	if(Opaque==0) return false;
	if (!Create(Opaque)) return false;
	char * rgbdata;
	GLuint w,h;
	FILE * fhandle=NULL;
	fhandle=fopen(Opaque,"rb");
	if (!fhandle) return false;
	char ww,hh;
	fread(&ww,sizeof(char),1,fhandle);
	w=pow2(ww-48);
	fread(&hh,sizeof(char),1,fhandle);
	h=pow2(hh-48);
	GLuint size=w*h*3;
	rgbdata=new char[size];
	GLuint read=fread(rgbdata,sizeof(char),size,fhandle);
	if (read!=size) return false;
	fclose(fhandle);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,w,h,GL_RGB,GL_UNSIGNED_BYTE,rgbdata);
	delete [] rgbdata;
	return true;
}
