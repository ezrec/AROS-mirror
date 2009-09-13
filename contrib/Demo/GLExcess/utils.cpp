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
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

void getErr()
{
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR)
	{
		errString = gluErrorString(errCode);
		printf((char *)errString);
	}
}
char tochar(int cipher)
{
	switch(cipher)
	{
		case 0: return '0'; break;
		case 1: return '1'; break;
		case 2: return '2'; break;
		case 3: return '3'; break;
		case 4: return '4'; break;
		case 5: return '5'; break;
		case 6: return '6'; break;
		case 7: return '7'; break;
		case 8: return '8'; break;
		case 9: return '9'; break;
		default: return '\0';
	}
}
char * tostr(int val)
{
	bool minzero=false;
	if (val<0)
	{
		val=-val;
		minzero=true;
	}

	int * ciphers;
	char * str;
	int num=(int)(log10(val)+1);
	if (minzero) num++;
	if (val==0) return "0";
	ciphers=new int [num+1];
	str = new char[num+1];
	int d=1;
	for (int i=1; i<=num; i++)
	{
		d=1;
		for (int c=1; c<=i; c++) d*=10;
		ciphers[i]=(val%d)/(d/10);
		if (minzero) str[num-1-i]=tochar(ciphers[i]);
		else str[num-i]=tochar(ciphers[i]);
	}
	if (minzero) {str [num-1]='-';str[num]='\0';} else str [num]='\0';
	return str;
}
int power(int base,int exp)
{
	int res=1;
	for (int i=0; i<exp; i++) res*=base;
	return res;
}
int toint(char* str)
{
	int res=0;
	for (int i=strlen(str)-1; i>=0; i--)
	{
		char ch=str[strlen(str)-1-i];
		res+=power(10,i)*(ch-48);
	}
	return res;
}
char * buildgamestring(int* params)
{
	char * res;
	res=strcat(tostr(params[1]),"x");
	res=strcat(res,tostr(params[2]));
	res=strcat(res,":");
	res=strcat(res,tostr(params[3]));
	res=strcat(res,"@");
	res=strcat(res,tostr(params[4]));
	return res;
}
