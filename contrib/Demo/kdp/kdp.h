/* KDP v0.7 by wrecK/KnP YhdZ/KnP

   hmmm......
*/

#ifndef KDP_H
#define KDP_H


#ifndef PI
  #define PI 3.1415926535897932384626433832795
#endif

#include <exec/types.h>

#include <stdio.h>

typedef struct
	{
	UBYTE *vmem;
	} KDPscreen;

typedef struct
	{
	int xspeed,yspeed;
	int button;
	BYTE x,y;
	} KDPmouse;


int   KDPopen(KDPscreen *screen);
void  KDPclose(KDPscreen *screen);
void  KDPsetColor(UBYTE num,UBYTE r,UBYTE g,UBYTE b);
void  KDPsetPal(UBYTE *pal);
void  KDPwaitVr();
void  KDPshow(UBYTE *vmem);
void  KDPgetMouse(KDPmouse *mouse);
void *KDPloadFile(char *filename);
WORD  KDPgetMword(UBYTE *wrd);
LONG  KDPgetMlong(UBYTE *lng);
WORD  KDPgetIword(UBYTE *wrd);
LONG  KDPgetIlong(UBYTE *lng);
float KDPgetMfloat(UBYTE *b);
float KDPgetIfload(UBYTE *b);
int   KDPwriteMword(WORD word,FILE *file);
int   KDPwriteMlong(LONG llng,FILE *file);
int   KDPwriteIword(WORD word,FILE *file);
int   KDPwriteIlong(LONG llng,FILE *file);



#endif /*KDP_H */
