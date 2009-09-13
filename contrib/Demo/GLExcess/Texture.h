#ifndef TEXTURE_H
#define TEXTURE_H
#define MAX_TEXTURE_NAME_LENGTH 64

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

class Texture
{
	public:
		Texture();
		~Texture();
		bool Load(char* Opaque);
		void Kill();
		void Use();

	protected:
		bool Create(char* FileName);
		char Name[MAX_TEXTURE_NAME_LENGTH];
		GLuint tID;
		GLuint Width, Height;
};
#endif

