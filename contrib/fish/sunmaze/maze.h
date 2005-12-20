#include <proto/alib.h>

#define StackX *(stack+(stackptr<<1))
#define StackY *(stack+(stackptr<<1)+1)

/* MAZETEST (xpos,ypos,mazewidth,pointer to buffer) */
#ifdef BITMAP
#define MAZETEST(a,b,c,d) (*((d)+((short)(b)*(short)((c)>>3))+((a)>>3)) & 0x80 >> ((a) & 7))
#else
#define MAZETEST(a,b,c,d) (*(d)+(a)+((b)*(c))))
#endif

/* MAZESET (xpos,ypos,mazewidth,pointer to buffer */
#ifdef BITMAP
#define MAZESET(a,b,c,d) *((d)+((short)(b)*(short)((c)>>3))+((a)>>3))|= 0x80 >> ((a) & 7)
#else
#define MAZESET(a,b,c,d) *((d)+(a)+((b)*(c)))=1
#endif

long seed;                     /* FastRand() stuff */
short testpoint[4][2]=
 {{-1,0},{1,0},{0,1},{0,-1}};  /* = left,right,down,up */
short stackptr,direction,newpos();
long *vposr = (long*)0xdff004; /* Beam position */

short maze(short width,short height,char * area)
{
 register short x,y;
 register short *stack;
 short i,j;

 //seed=*vposr;           /* new Seed for FastRand() */
 x=2;                   /* starting position */
 y=(FastRand(seed) % (height-4) + 2) & 0xfffe;

  /* we need a 'stack' to store those positions, from where we can */
  /* continue if we get into a dead end */
 if(!(stack=(short*) AllocMem(width*height,NULL))) return(NULL);

 stackptr=1;
 StackX=x;StackY=y;

 j= width & 1 ? width-1:width-2;
 for(i=0;i<height;i++)          /* surround the 'maze' with 'passages' */
 {
   MAZESET(0,i,width,area);
   MAZESET(j,i,width,area);
 }
 j= height & 1 ? height-1:height-2;
 for(i=0;i<width;i++)
 {
   MAZESET(i,0,width,area);
   MAZESET(i,j,width,area);
 }

 MAZESET(x,y,width,area);         /* set the first position to 'passage' */

 while(stackptr)                  /* do we have a untested position left ? */
 {
  direction=seed=FastRand(seed);  /* pick up a random direction */

  for (i=0;i<4;i++,direction++)   /* test all 4 directions */
  {
   direction &= 3;                /* = %4 */
   /* test if the position we are moving to is already a 'passage' */
   if(! MAZETEST(x+2*testpoint[direction][0],2*testpoint[direction][1]+y,width,area))
   {
    /* if not move to the new position */
    MAZESET(x+testpoint[direction][0],testpoint[direction][1]+y,width,area);
    x+=2*testpoint[direction][0];
    y+=2*testpoint[direction][1];
    MAZESET(x,y,width,area);
    StackX=x;                     /* stack current position because */
    StackY=y;                     /* there may be more than one direction */
    stackptr++;                   /* we can move to */
    goto endtest;                 /* GOTO ?!? */
   }
  }
  stackptr--;                   /* dead-end, delete this point from stack */
  x=StackX;                     /* test last position again */
  y=StackY;
  endtest:
  	; /* Inserted ; to make Aztec happy -- PDS(3) -- 31-jul-88 */
 }
 FreeMem(stack,width*height);
 return(1);
}


